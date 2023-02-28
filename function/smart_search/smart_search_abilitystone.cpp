#include "smart_search_abilitystone.h"
#include "ui_smart_search_abilitystone.h"
#include "ui/widget_manager.h"
#include "ui/engrave_selector.h"
#include "api/api_manager.h"
#include "api/search_option.h"
#include "game_data/character/engrave/engrave_manager.h"

#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>

SmartSearchAbilityStone::SmartSearchAbilityStone(QLayout* pLayout) :
    ui(new Ui::SmartSearchAbilityStone),
    m_addCount(0)
{
    ui->setupUi(this);
    pLayout->addWidget(this);
    this->hide();

    ui->vLayoutMain->setAlignment(Qt::AlignHCenter);
    ui->hLayoutMenu->setAlignment(Qt::AlignHCenter);

    initMenu();
    initResultLayout();
}

SmartSearchAbilityStone::~SmartSearchAbilityStone()
{
    for (EngraveSelector* pEngraveSelector : m_engraveSelectors)
        delete pEngraveSelector;
    for (QWidget* pWidget : m_widgets)
        delete pWidget;
    for (QPushButton* pButton : m_engraveSelectButtons)
        delete pButton;
    for (QLayout* pLayout : m_layouts)
        delete pLayout;
    delete ui;
}

void SmartSearchAbilityStone::refresh()
{
    // Nothing to do
}

void SmartSearchAbilityStone::initMenu()
{
    const int MAX_SELECTOR = 5;
    const QString selectedButtonStyle = "QPushButton { "
                                        "border: 2px solid rgb(58, 134, 255);"
                                        "border-radius: 5px;"
                                        "background-color: rgb(225, 225, 225);"
                                        "}";

    // 선택 초기화 버튼 추가
    QPushButton* pButtonInit = WidgetManager::createPushButton("초기화");
    pButtonInit->setStyleSheet("QPushButton { color: red }");
    ui->hLayoutMenu->addWidget(pButtonInit);
    connect(pButtonInit, &QPushButton::released, this, [&](){
        for (QPushButton* pButton : m_engraveSelectButtons)
        {
            pButton->setText("각인 선택");
            pButton->setStyleSheet("");
        }
    });
    m_widgets.append(pButtonInit);

    // 각인 선택 버튼 추가
    for (int i = 0; i < MAX_SELECTOR; i++)
    {
        // EngraveSelector 생성
        EngraveSelector* pEngraveSelector = new EngraveSelector(false);
        m_engraveSelectors.append(pEngraveSelector);

        // EngraveSelector popup 버튼 생성
        QPushButton* pButtonSelector = WidgetManager::createPushButton("각인 선택");
        ui->hLayoutMenu->addWidget(pButtonSelector);
        connect(pButtonSelector, &QPushButton::released, this, [&, pEngraveSelector](){
            pEngraveSelector->show();
        });
        m_engraveSelectButtons.append(pButtonSelector);

        // 각인 선택 완료 시 동작 정의
        QList<QPushButton*> buttons = pEngraveSelector->getButtons();
        for (QPushButton* pButton : buttons)
        {
            // EngraveSelector와 연결된 버튼에 선택한 각인 이름을 표기
            // 선택 완료 시 스타일 변경
            connect(pButton, &QPushButton::released, this, [&, pButton, pButtonSelector, selectedButtonStyle, pEngraveSelector](){
                pButtonSelector->setText(pButton->text());
                pButtonSelector->setStyleSheet(selectedButtonStyle);
                pEngraveSelector->hide();
            });
        }
    }

    // 검색 버튼 추가
    QPushButton* pButtonSearch = WidgetManager::createPushButton("검색");
    connect(pButtonSearch, &QPushButton::released, this, &SmartSearchAbilityStone::searchAbilityStone);
    ui->hLayoutMenu->addWidget(pButtonSearch);
    m_widgets.append(pButtonSearch);
}

void SmartSearchAbilityStone::initResultLayout()
{
    const QStringList attributes = {"", "#", "각인1", "각인2", "최저가"};
    const int widths[5] = {25, 50, 100, 100, 100};

    for (int col = 0; col < attributes.size(); col++)
    {
        QLabel* pLabelAttribute = WidgetManager::createLabel(attributes[col], 12, "", widths[col]);
        ui->gridResult->addWidget(pLabelAttribute, 0, col);
        m_widgets.append(pLabelAttribute);
    }
}

void SmartSearchAbilityStone::searchAbilityStone()
{
    // 선택한 각인 취합 (중복X)
    QSet<QString> engraves;
    for (const QPushButton* pButton : m_engraveSelectButtons)
    {
        const QString& text = pButton->text();
        if (text != "각인 선택")
            engraves.insert(text);
    }

    // 선택된 각인이 2개 미만이면 검색 취소
    if (engraves.size() < 2)
    {
        QMessageBox msgBox;
        msgBox.setText("각인을 2개 이상 선택해주세요.");
        msgBox.exec();
        return;
    }

    // 가능한 모든 조합으로 검색
    const QStringList selectedEngraves = engraves.values();
    for (int i = 0; i < selectedEngraves.size() - 1; i++)
    {
        for (int j = i + 1; j < selectedEngraves.size(); j++)
        {
            const QString& engrave1 = selectedEngraves[i];
            const QString& engrave2 = selectedEngraves[j];

            QNetworkAccessManager* pNetworkManager = new QNetworkAccessManager();
            connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&, engrave1, engrave2](QNetworkReply* pReply){
                QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
                if (response.isNull())
                    return;

                // 검색 결과 parsing (최저가 1개)
                const QJsonObject& item = response.object().find("Items")->toArray()[0].toObject();
                const int& price = item.find("AuctionInfo")->toObject().find("BuyPrice")->toInt();
                // 검색 결과 추가
                addSearchResult({engrave1, engrave2}, price);
            });
            connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

            // 어빌리티 스톤 검색 옵션 세팅 후 api 요청 전달
            SearchOption searchOption(SearchType::Auction);
            searchOption.setCategoryCode(CategoryCode::AbilityStone);
            searchOption.setItemGrade(ItemGrade::유물);
            searchOption.setItemTier(3);
            searchOption.setPageNo(1);
            searchOption.setSortCondition("ASC");
            searchOption.setEtcOption(ENGRAVE_OPTION_CODE, EngraveManager::getInstance()->getEngraveCode(engrave1));
            searchOption.setEtcOption(ENGRAVE_OPTION_CODE, EngraveManager::getInstance()->getEngraveCode(engrave2));

            ApiManager::getInstance()->post(pNetworkManager, LostarkApi::Auction, QJsonDocument(searchOption.toJsonObject()).toJson());
        }
    }
}

void SmartSearchAbilityStone::addSearchResult(const QStringList& engraves, const int& price)
{
    const QString iconAbilityStone = ":/item/image/item/abilitystone_0.png";
    const QString iconCancle = ":/icon/image/cancle.png";

    // 아이콘 추가
    QLabel* pIcon = WidgetManager::createIcon(iconAbilityStone, nullptr, backgroundColorCode(ItemGrade::유물));
    ui->gridResult->addWidget(pIcon, ++m_addCount, 1, Qt::AlignHCenter);

    // 각인명 추가
    QList<QLabel*> engraveLabels;
    for (int i = 0; i < 2; i++)
    {
        QLabel* pLabelEngrave = WidgetManager::createLabel(engraves[i]);
        ui->gridResult->addWidget(pLabelEngrave, m_addCount, i + 2);
        engraveLabels.append(pLabelEngrave);
    }

    // 가격 추가
    QLabel* pLabelPrice = WidgetManager::createLabel(QString("%L1").arg(price));
    ui->gridResult->addWidget(pLabelPrice, m_addCount, 4);

    // 삭제 버튼 추가
    QPushButton* pButtonDelete = WidgetManager::createPushButton(QPixmap(iconCancle), 25, 25);
    ui->gridResult->addWidget(pButtonDelete, m_addCount, 0);
    connect(pButtonDelete, &QPushButton::released, this, [&, pIcon, engraveLabels, pLabelPrice, pButtonDelete](){
        delete pIcon;
        for (QLabel* pLabelEngrave : engraveLabels)
            delete pLabelEngrave;
        delete pLabelPrice;
        delete pButtonDelete;
    });
}
