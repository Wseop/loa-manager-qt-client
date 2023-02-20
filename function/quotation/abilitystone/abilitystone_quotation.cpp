#include "abilitystone_quotation.h"
#include "ui_abilitystone_quotation.h"
#include "ui/widget_manager.h"
#include "ui/engrave_selector.h"
#include "game_data/character/engrave/engrave_manager.h"
#include "api/api_manager.h"
#include "function/quotation/abilitystone/abilitystone_price.h"
#include <QPushButton>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>

AbilityStoneQuotation* AbilityStoneQuotation::m_pInstance = nullptr;

AbilityStoneQuotation::AbilityStoneQuotation() :
    ui(new Ui::AbilityStoneQuotation)
{
    ui->setupUi(this);

    setAlignments();
    addEngraveSelectors();
}

AbilityStoneQuotation::~AbilityStoneQuotation()
{
    delete ui;
}

void AbilityStoneQuotation::setAlignments()
{
    ui->hLayoutInput->setAlignment(Qt::AlignHCenter);
    ui->vLayoutOutput->setAlignment(Qt::AlignHCenter);
}

void AbilityStoneQuotation::addEngraveSelectors()
{
    const int MAX_SELECT_BUTTON = 5;
    const QString selectedButtonStyle = "QPushButton {"
                                        "border: 2px solid rgb(58, 134, 255);"
                                        "border-radius: 5px;"
                                        "background-color: rgb(225, 225, 225);"
                                        "}";

    // 초기화 버튼 추가
    QPushButton* pInitButton = WidgetManager::createPushButton("초기화");
    pInitButton->setStyleSheet("QPushButton { color: red }");
    ui->hLayoutInput->addWidget(pInitButton);
    connect(pInitButton, &QPushButton::released, this, [&](){
        for (int i = 0; i < MAX_SELECT_BUTTON; i++)
        {
            QPushButton* pButton = m_engraveSelectButtons[i];
            pButton->setText(QString("각인 선택 %1").arg(i + 1));
            pButton->setStyleSheet("");
        }
    });

    // 각인 선택 버튼 추가
    for (int i = 0; i < MAX_SELECT_BUTTON; i++)
    {
        QHBoxLayout* pHLayout = new QHBoxLayout();
        ui->hLayoutInput->addLayout(pHLayout);

        EngraveSelector* pEngraveSelector = new EngraveSelector(false);
        m_engraveSelectors.append(pEngraveSelector);
        QPushButton* pButton = WidgetManager::createPushButton(QString("각인 선택 %1").arg(i + 1));
        m_engraveSelectButtons.append(pButton);
        pHLayout->addWidget(pButton);

        connect(pButton, &QPushButton::released, this, [&, pEngraveSelector](){
            pEngraveSelector->show();
        });
        QList<QPushButton*> engraveButtons = pEngraveSelector->getButtons();
        for (QPushButton* pEngraveButton : engraveButtons)
        {
            connect(pEngraveButton, &QPushButton::released, this, [&, pEngraveSelector, pEngraveButton, pButton, selectedButtonStyle](){
                pButton->setText(pEngraveButton->text());
                pButton->setStyleSheet(selectedButtonStyle);
                pEngraveSelector->hide();
            });
        }
    }

    // 검색 버튼 추가
    QPushButton* pSearchButton = WidgetManager::createPushButton("검색");
    ui->hLayoutInput->addWidget(pSearchButton);
    connect(pSearchButton, &QPushButton::released, this, &AbilityStoneQuotation::search);
}

void AbilityStoneQuotation::search()
{
    m_selectedEngraves.clear();

    // 선택된 각인 탐색
    for (QPushButton* pButton : m_engraveSelectButtons)
    {
        const QString& text = pButton->text();
        if (text.contains("각인 선택"))
            continue;
        m_selectedEngraves.insert(pButton->text());
    }

    // 선택한 각인이 2개 미만이면 알람을 띄우고 탐색 중지
    if (m_selectedEngraves.size() < 2)
    {
        QMessageBox msgBox;
        msgBox.setText("각인을 2개 이상 선택해주세요.");
        msgBox.exec();
    }
    else
    {
        // 가능한 모든 조합으로 조회
        QStringList selectedEngraves(m_selectedEngraves.begin(), m_selectedEngraves.end());
        for (int i = 0; i < selectedEngraves.size() - 1; i++)
        {
            for (int j = i + 1; j < selectedEngraves.size(); j++)
            {
                sendRequest(selectedEngraves[i], selectedEngraves[j]);
            }
        }
    }
}

void AbilityStoneQuotation::sendRequest(QString engrave1, QString engrave2)
{
    ApiManager* pApiManager = ApiManager::getInstance();
    EngraveManager* pEngraveManager = EngraveManager::getInstance();

    QNetworkAccessManager* pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&](QNetworkReply* pReply){
        QJsonDocument result = QJsonDocument::fromJson(pReply->readAll());
        if (result.isNull())
            return;

        // 최저가로 등록된 아이템 1개만 parsing
        QJsonArray items = result.object().find("Items")->toArray();
        if (items.size() == 0)
            return;
        QJsonObject item = items[0].toObject();
        QJsonArray itemOptions = item.find("Options")->toArray();
        QString engrave1 = itemOptions[0].toObject().find("OptionName")->toString();
        QString engrave2 = itemOptions[1].toObject().find("OptionName")->toString();
        int price = item.find("AuctionInfo")->toObject().find("BuyPrice")->toInt();

        addResult(engrave1, engrave2, price);
    });
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    QList<int> engraveCodes = {pEngraveManager->getEngraveCode(engrave1), pEngraveManager->getEngraveCode(engrave2)};
    QJsonObject searchOption = pApiManager->buildSearchOption(SearchType::Auction, CategoryCode::AbilityStone, {3, 3}, engraveCodes);
    pApiManager->post(pNetworkManager, LostarkApi::Auction, QJsonDocument(searchOption).toJson());
}

void AbilityStoneQuotation::addResult(QString engrave1, QString engrave2, int price)
{
    QHBoxLayout* pHLayout = new QHBoxLayout();
    ui->vLayoutOutput->addLayout(pHLayout);

    QPushButton* pRemoveButton = WidgetManager::createPushButton(QPixmap(":/icon/image/cancle.png"), 10, 25, 25);
    pHLayout->addWidget(pRemoveButton);

    AbilityStonePrice* pResult = new AbilityStonePrice(engrave1, engrave2, price);
    pHLayout->addWidget(pResult);

    connect(pRemoveButton, &QPushButton::released, this, [&, pHLayout, pRemoveButton, pResult](){
        delete pResult;
        delete pRemoveButton;
        delete pHLayout;
    });
}

AbilityStoneQuotation* AbilityStoneQuotation::getInstance()
{
    if (m_pInstance == nullptr)
        m_pInstance = new AbilityStoneQuotation();
    return m_pInstance;
}

void AbilityStoneQuotation::destroyInstance()
{
    if (m_pInstance == nullptr)
        return;
    delete m_pInstance;
    m_pInstance = nullptr;
}
