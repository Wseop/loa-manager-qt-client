#include "raid_profit.h"
#include "ui_raid_profit.h"
#include "resource/resource_manager.h"
#include "api/api_manager.h"
#include "ui/widget_manager.h"
#include "function/raid_profit/raid_profit_table.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QCheckBox>

RaidProfit *RaidProfit::m_pInstance = nullptr;

RaidProfit::RaidProfit() :
    ui(new Ui::RaidProfit),
    m_responseCount(0),
    m_pContentSelector(nullptr),
    m_pCurrentTable(nullptr)
{
    ui->setupUi(this);
    ui->hLayoutSelector->setAlignment(Qt::AlignHCenter);

    initializeRaidReward();
    initializeSearchOption();
    initializeContentSelect();
    initializeItemSelect();
    initializeProfitTable();
}

RaidProfit::~RaidProfit()
{
    for (SearchOption *pSearchOption : m_searchOptions)
        delete pSearchOption;
    m_searchOptions.clear();

    for (QWidget *pWidget : m_widgets)
        delete pWidget;
    m_widgets.clear();

    for (QLayout *pLayout : m_layouts)
        delete pLayout;
    m_layouts.clear();

    delete ui;
}

void RaidProfit::initializeRaidReward()
{
    QJsonObject json = ResourceManager::getInstance()->loadJson("raid_reward");

    // 전체 콘텐츠 목록 초기화
    m_contents = json.find("Contents")->toVariant().toStringList();

    // 아이템 가격 테이블 초기화
    const QStringList &rewards = json.find("Rewards")->toVariant().toStringList();
    for (const QString &reward : rewards)
    {
        if (reward.contains("명예의 파편"))
            m_itemPrice["명예의 파편"] = 0;
        else if (reward == "선명한 지혜의 정수")
            m_itemPrice["선명한 지혜의 기운"] = 0;
        else if (reward == "빛나는 지혜의 정수")
            m_itemPrice["빛나는 지혜의 기운"] = 0;
        else
            m_itemPrice[reward] = 0;
    }

    // 레이드 관문별 더보기 보상 정보 초기화
    for (const QString &content : m_contents)
    {
        const QJsonArray &more = json.find(content)->toObject().find("More")->toArray();
        QList<int> costsPerRaid;
        QList<RewardItems> rewardItemsPerRaid;

        for (const QJsonValue &value : more)
        {
            const QJsonObject &object = value.toObject();
            const QJsonArray &items = object.find("Items")->toArray();
            RewardItems rewardItemsPerGate;

            costsPerRaid << object.find("Cost")->toInt();

            for (const QJsonValue &value : items)
            {
                const QJsonObject &item = value.toObject();
                const QString &name = item.find("Name")->toString();
                int count = item.find("Count")->toInt();

                rewardItemsPerGate.append({name, count});
            }

            rewardItemsPerRaid << rewardItemsPerGate;
        }

        m_costs << costsPerRaid;
        m_rewardItems << rewardItemsPerRaid;
    }
}

void RaidProfit::initializeSearchOption()
{
    // Item 가격 검색 시 필요한 SearchOption 초기화
    const QStringList &items = m_itemPrice.keys();
    for (const QString &item : items)
    {
        SearchOption *pSearchOption = new SearchOption(SearchType::Market);
        pSearchOption->setCategoryCode(CategoryCode::Reforge);
        if (item == "명예의 파편")
            pSearchOption->setItemName("명예의 파편 주머니(소)");
        else if (item == "선명한 지혜의 기운")
            pSearchOption->setItemName("선명한 지혜의 정수");
        else if (item == "빛나는 지혜의 기운")
            pSearchOption->setItemName("빛나는 지혜의 정수");
        else
            pSearchOption->setItemName(item);
        pSearchOption->setItemTier(3);
        pSearchOption->setPageNo(1);
        pSearchOption->setSortCondition("ASC");

        m_searchOptions << pSearchOption;
    }
}

void RaidProfit::initializeContentSelect()
{
    QGroupBox *pGroupContentSelect = WidgetManager::createGroupBox("레이드 선택");
    ui->hLayoutSelector->addWidget(pGroupContentSelect);
    m_widgets << pGroupContentSelect;

    QHBoxLayout *pLayoutContentSelect = new QHBoxLayout();
    pGroupContentSelect->setLayout(pLayoutContentSelect);
    m_layouts << pLayoutContentSelect;

    m_pContentSelector = WidgetManager::createComboBox(m_contents);
    pLayoutContentSelect->addWidget(m_pContentSelector);
    m_widgets << m_pContentSelector;

    connect(m_pContentSelector, &QComboBox::currentIndexChanged, this, [&](int index){
        m_pCurrentTable->hide();
        m_pCurrentTable = m_profitTables[index];
        m_pCurrentTable->show();
    });
}

void RaidProfit::initializeItemSelect()
{
    QGroupBox *pGroupItemSelect = WidgetManager::createGroupBox("손익 계산에 포함");
    ui->hLayoutSelector->addWidget(pGroupItemSelect);
    m_widgets << pGroupItemSelect;

    QHBoxLayout *pLayoutItemSelect = new QHBoxLayout();
    pGroupItemSelect->setLayout(pLayoutItemSelect);
    m_layouts << pLayoutItemSelect;

    const QStringList items = {"명예의 파편", "파괴석", "수호석", "돌파석", "정수"};
    for (const QString &item : items)
    {
        QVBoxLayout *pLayout = new QVBoxLayout();
        pLayoutItemSelect->addLayout(pLayout);
        m_layouts << pLayout;

        QLabel *pLabel = WidgetManager::createLabel(item);
        pLayout->addWidget(pLabel);
        m_widgets << pLabel;

        QCheckBox *pItemSelector = new QCheckBox();
        pItemSelector->setChecked(true);
        pLayout->addWidget(pItemSelector);
        pLayout->setAlignment(pItemSelector, Qt::AlignHCenter);
        m_itemSelectors << pItemSelector;
        m_widgets << pItemSelector;

        connect(pItemSelector, &QCheckBox::stateChanged, this, [&](){
            for (RaidProfitTable *pProfitTable : m_profitTables)
                pProfitTable->refreshItemPrice(m_itemPrice, m_itemSelectors);
        });
    }
}

void RaidProfit::initializeProfitTable()
{
    for (int i = 0; i < m_contents.size(); i++)
    {
        RaidProfitTable *pProfitTable = new RaidProfitTable(m_contents[i], m_costs[i], m_rewardItems[i]);
        pProfitTable->hide();
        ui->vLayoutMain->addWidget(pProfitTable);

        m_profitTables << pProfitTable;
        m_widgets << pProfitTable;
    }

    m_pCurrentTable = m_profitTables[0];
    m_pCurrentTable->show();
}

void RaidProfit::refreshItemPrice()
{
    m_responseCount = 0;

    for (int i = 0; i < m_itemPrice.size(); i++)
    {
        QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
        connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&](QNetworkReply *pReply){
            m_responseCount++;

            // 아이템 가격 검색 결과 처리
            QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
            if (!response.isNull())
            {
                const QJsonObject &item = response.object().find("Items")->toArray().first().toObject();
                const QString &itemName = item.find("Name")->toString();
                int itemPrice = item.find("CurrentMinPrice")->toInt();

                if (itemName.contains("명예의 파편"))
                    m_itemPrice["명예의 파편"] = itemPrice / (double)500;
                else if (itemName == "선명한 지혜의 정수")
                    m_itemPrice["선명한 지혜의 기운"] = itemPrice * 1.5;
                else if (itemName == "빛나는 지혜의 정수")
                    m_itemPrice["빛나는 지혜의 기운"] = itemPrice * 1.5;
                else if (itemName.contains("파괴") || itemName.contains("수호"))
                    m_itemPrice[itemName] = itemPrice / (double)10;
                else
                    m_itemPrice[itemName] = itemPrice;
            }

            // 모든 아이템 검색 완료 시 UI Update
            if (m_responseCount == m_itemPrice.size())
            {
                for (RaidProfitTable *pProfitTable : m_profitTables)
                    pProfitTable->refreshItemPrice(m_itemPrice, m_itemSelectors);
            }
        });
        connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

        ApiManager::getInstance()->post(pNetworkManager, LostarkApi::Market, QJsonDocument(m_searchOptions[i]->toJsonObject()).toJson());
    }
}

RaidProfit *RaidProfit::getInstance()
{
    if (m_pInstance == nullptr)
        m_pInstance = new RaidProfit();

    return m_pInstance;
}

void RaidProfit::destroyInstance()
{
    if (m_pInstance == nullptr)
        return;

    delete m_pInstance;
    m_pInstance = nullptr;
}

void RaidProfit::start()
{
    refreshItemPrice();
}
