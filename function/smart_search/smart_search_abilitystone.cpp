#include "smart_search_abilitystone.h"
#include "ui_smart_search_abilitystone.h"
#include "ui/widget_manager.h"
#include "api/api_manager.h"
#include "api/search_option.h"
#include "game/engrave/engrave_manager.h"
#include "util/util.h"

#include <algorithm>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>

SmartSearchAbilityStone::SmartSearchAbilityStone(QLayout *pLayout) :
    ui(new Ui::SmartSearchAbilityStone),
    m_searchResults(0),
    m_totalSearchCount(0),
    m_currentSearchCount(0)
{
    ui->setupUi(this);
    ui->hLayoutMenu->setAlignment(Qt::AlignHCenter);

    pLayout->addWidget(this);
    this->hide();

    initializeEngraveSelector();
    initializeResultUI();
}

SmartSearchAbilityStone::~SmartSearchAbilityStone()
{
    clearResult();

    for (QComboBox* pEngraveSelector : m_engraveSelectors)
        delete pEngraveSelector;
    delete m_pPenaltySelector;

    for (QWidget* pWidget : m_widgets)
        delete pWidget;

    for (QLayout* pLayout : m_layouts)
        delete pLayout;

    delete ui;
}

void SmartSearchAbilityStone::refresh()
{
    if (m_searchResults.size() == 0)
        return;

    // 검색 결과 최저가 순으로 정렬
    std::sort(m_searchResults.begin(), m_searchResults.end(), [](const auto& a, const auto& b){
        return a.second.buyPrice < b.second.buyPrice;
    });

    // 검색 결과 출력
    int row = 0;
    for (const auto& searchResult : m_searchResults)
    {
        const AbilityStone &abilityStone = searchResult.first;
        const Price &price = searchResult.second;

        QFrame *pHLine = WidgetManager::createLine(QFrame::HLine);
        ui->gridResult->addWidget(pHLine, ++row, 0, 1, -1);
        m_resultWidgets.append(pHLine);

        QLabel *pIcon = WidgetManager::createIcon(abilityStone.iconPath(), nullptr, itemGradeToBGColor(abilityStone.itemGrade()));
        ui->gridResult->addWidget(pIcon, ++row, 0);
        m_resultWidgets.append(pIcon);

        QLabel *pLabelName = WidgetManager::createLabel(abilityStone.itemName(), 10, itemGradeToTextColor(abilityStone.itemGrade()));
        ui->gridResult->addWidget(pLabelName, row, 1);
        m_resultWidgets.append(pLabelName);

        const QStringList &engraves = abilityStone.getEngraves();
        for (int i = 0; i < engraves.size(); i++)
        {
            QLabel *pLabelEngrave = WidgetManager::createLabel(engraves[i]);
            ui->gridResult->addWidget(pLabelEngrave, row, 2 + i);
            m_resultWidgets.append(pLabelEngrave);
        }

        QLabel *pLabelPenalty = WidgetManager::createLabel(abilityStone.getPenalties().at(0), 10, "red");
        ui->gridResult->addWidget(pLabelPenalty, row, 4);
        m_resultWidgets.append(pLabelPenalty);

        QLabel *pLabelPrice = WidgetManager::createLabel(QString("%L1\n(%L2)").arg(price.buyPrice).arg(price.bidStartPrice), 10, "", 200, 50);
        ui->gridResult->addWidget(pLabelPrice, row++, 5);
        m_resultWidgets.append(pLabelPrice);
    }
}

void SmartSearchAbilityStone::initializeEngraveSelector()
{
    const int MAX_SELECTOR = 5;

    // 전투 각인 오름차순 정렬
    QStringList battleEngraves = EngraveManager::getInstance()->getBattleEngraves();
    std::sort(battleEngraves.begin(), battleEngraves.end());

    // 각인 선택기 추가
    QStringList engraves = {"선택 안함"};
    engraves << battleEngraves;

    QGroupBox *pGroupEngrave = WidgetManager::createGroupBox("각인 선택");
    ui->hLayoutMenu->addWidget(pGroupEngrave);
    m_widgets.append(pGroupEngrave);

    QHBoxLayout *pLayoutGroupEngrave = new QHBoxLayout();
    pGroupEngrave->setLayout(pLayoutGroupEngrave);
    m_layouts.append(pLayoutGroupEngrave);

    for (int i = 0; i < MAX_SELECTOR; i++)
    {
        QComboBox *pEngraveSelector = WidgetManager::createComboBox(engraves);
        pLayoutGroupEngrave->addWidget(pEngraveSelector);
        m_engraveSelectors.append(pEngraveSelector);
    }

    // 감소 각인 선택기 추가
    QStringList penalties = {"선택 안함"};
    penalties << EngraveManager::getInstance()->getPenalties();

    QGroupBox *pGroupPenalty = WidgetManager::createGroupBox("감소 각인 선택");
    ui->hLayoutMenu->addWidget(pGroupPenalty);
    m_widgets.append(pGroupPenalty);

    QHBoxLayout *pLayoutGroupPenalty = new QHBoxLayout();
    pGroupPenalty->setLayout(pLayoutGroupPenalty);
    m_layouts.append(pLayoutGroupPenalty);

    m_pPenaltySelector = WidgetManager::createComboBox(penalties);
    pLayoutGroupPenalty->addWidget(m_pPenaltySelector);

    // 검색 버튼 추가
    QPushButton *pButtonSearch = WidgetManager::createPushButton("검색");
    connect(pButtonSearch, &QPushButton::released, this, &SmartSearchAbilityStone::searchAbilityStone);
    ui->hLayoutMenu->addWidget(pButtonSearch);
    m_widgets.append(pButtonSearch);
}

void SmartSearchAbilityStone::initializeResultUI()
{
    const QStringList attributes = {"#", "아이템명", "각인1", "각인2", "감소", "즉시 구매가\n(최소 입찰가)"};

    for (int col = 0; col < attributes.size(); col++)
    {
        QLabel *pLabelAttribute = WidgetManager::createLabel(attributes[col], 12, "", 200, 50);
        ui->gridResult->addWidget(pLabelAttribute, 0, col);
        m_widgets.append(pLabelAttribute);
    }
}

void SmartSearchAbilityStone::searchAbilityStone()
{
    // 선택한 각인 취합 (중복X)
    QSet<QString> engraves;
    for (const QComboBox* pEngraveSelector : m_engraveSelectors)
    {
        if (pEngraveSelector->currentIndex() != 0)
            engraves.insert(pEngraveSelector->currentText());
    }

    // 선택된 각인이 2개 미만이면 검색 취소
    if (engraves.size() < 2)
    {
        QMessageBox msgBox;
        msgBox.setText("각인을 2개 이상 선택해주세요.");
        msgBox.exec();
        return;
    }

    clearResult();

    // 가능한 모든 조합으로 검색
    const QStringList selectedEngraves = engraves.values();
    m_totalSearchCount = combination(selectedEngraves.size(), 2);

    for (int i = 0; i < selectedEngraves.size() - 1; i++)
    {
        for (int j = i + 1; j < selectedEngraves.size(); j++)
        {
            const QString &engrave1 = selectedEngraves[i];
            const QString &engrave2 = selectedEngraves[j];

            QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
            connect(pNetworkManager, &QNetworkAccessManager::finished, this, &SmartSearchAbilityStone::parseSearchResult);
            connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

            // 어빌리티 스톤 검색 옵션 세팅 후 api 요청 전달
            SearchOption searchOption(SearchType::Auction);
            searchOption.setCategoryCode(CategoryCode::AbilityStone);
            searchOption.setItemGrade(ItemGrade::유물);
            searchOption.setItemTier(3);
            searchOption.setPageNo(1);
            searchOption.setSortCondition("ASC");
            searchOption.setEtcOption(EtcOptionCode::Engrave, EngraveManager::getInstance()->getEngraveCode(engrave1));
            searchOption.setEtcOption(EtcOptionCode::Engrave, EngraveManager::getInstance()->getEngraveCode(engrave2));
            if (m_pPenaltySelector->currentIndex() != 0)
                searchOption.setEtcOption(EtcOptionCode::Engrave, EngraveManager::getInstance()->getEngraveCode(m_pPenaltySelector->currentText()));

            ApiManager::getInstance()->post(pNetworkManager, LostarkApi::Auction, QJsonDocument(searchOption.toJsonObject()).toJson());
        }
    }
}

void SmartSearchAbilityStone::parseSearchResult(QNetworkReply *pReply)
{
    m_currentSearchCount++;

    QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
    if (response.isNull())
        return;

    // 검색 결과 parsing (최저가 1개)
    const QJsonObject &item = response.object().find("Items")->toArray()[0].toObject();
    const QJsonObject &auctionInfo = item.find("AuctionInfo")->toObject();
    int buyPrice = auctionInfo.find("BuyPrice")->toInt();
    int bidStartPrice = auctionInfo.find("BidStartPrice")->toInt();

    AbilityStone abilityStone;
    abilityStone.setItemName(item.find("Name")->toString());
    abilityStone.setItemGrade(qStringToItemGrade(item.find("Grade")->toString()));
    abilityStone.setIconPath(":/image/item/abilitystone/0.png");

    const QJsonArray &options = item.find("Options")->toArray();
    for (const QJsonValue& value : options)
    {
        const QJsonObject &option = value.toObject();
        const QString &engrave = option.find("OptionName")->toString();

        if (option.find("IsPenalty")->toBool())
            abilityStone.addPenalty(engrave, 0);
        else
            abilityStone.addEngrave(engrave, 0);
    }

    // 검색 결과 추가
    m_searchResults.append({abilityStone, {buyPrice, bidStartPrice}});

    // 검색 완료 시 UI 갱신
    if (m_totalSearchCount == m_currentSearchCount)
        refresh();
}

void SmartSearchAbilityStone::clearResult()
{
    m_searchResults.clear();

    m_totalSearchCount = 0;
    m_currentSearchCount = 0;

    for (QWidget* pWidget : m_resultWidgets)
        delete pWidget;
    m_resultWidgets.clear();
}
