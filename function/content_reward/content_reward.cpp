#include "content_reward.h"
#include "ui_content_reward.h"
#include "ui/widget_manager.h"
#include "resource/resource_manager.h"
#include "api/api_manager.h"
#include "api/search_option.h"
#include "db/db_manager.h"
#include "function/content_reward/content_reward_table.h"
#include "function/content_reward/content_reward_adder.h"

#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>

extern bool g_bAdmin;

ContentReward *ContentReward::m_pInstance = nullptr;

ContentReward::ContentReward() :
    ui(new Ui::ContentReward),
    m_pContentSelector(nullptr),
    m_pSelectedTable(nullptr),
    m_pRewardAdder(nullptr)
{
    ui->setupUi(this);
    ui->hLayoutSelector->setAlignment(Qt::AlignHCenter);

    initializeContentData();
    initializeSearchOption();
    initializeContentSelector();
    initializeTradableSelector();
    initializeRewardTable();
    initializeRewardAdder();
}

ContentReward::~ContentReward()
{
    for (SearchOption *pSearchOption : m_searchOptions)
        delete pSearchOption;
    m_searchOptions.clear();

    if (m_pContentSelector != nullptr)
    {
        delete m_pContentSelector;
        m_pContentSelector = nullptr;
    }

    for (auto iter = m_tradableSelector.begin(); iter != m_tradableSelector.end(); iter++)
        delete iter.value();
    m_tradableSelector.clear();

    for (ContentRewardTable *pRewardTable : m_rewardTables)
        delete pRewardTable;
    m_rewardTables.clear();

    if (m_pRewardAdder != nullptr)
        delete m_pRewardAdder;
    m_pRewardAdder = nullptr;

    for (QWidget *pWidget : m_widgets)
        delete pWidget;
    m_widgets.clear();

    for (QLayout *pLayout : m_layouts)
        delete pLayout;
    m_layouts.clear();

    delete ui;
}

void ContentReward::initializeContentData()
{
    QJsonObject json = ResourceManager::getInstance()->loadJson("drop_table");

    // 컨텐츠 목록 초기화
    m_contents = json.find("Contents")->toVariant().toStringList();

    // 컨텐츠별 data 초기화
    for (const QString &content : m_contents)
    {
        const QJsonArray &dropTable = json.find(content)->toObject().find("DropTable")->toArray();

        for (const QJsonValue &value : dropTable)
        {
            const QJsonObject &object = value.toObject();
            const QString &level = object.find("Level")->toString();
            int stage = object.find("Stage")->toInt();

            m_contentLevels[content] << level;
            m_dropTable[level] = object.find("ItemList")->toVariant().toStringList();

            for (int i = 0; i < stage; i++)
            {
                const QString levelStage = content == "카오스 던전" ? level + QString::number(i + 1) : level;
                m_rewardData[levelStage] = {0, QList<int>(m_dropTable[level].size(), 0)};
            }
        }
    }

    // 골드로 환산이 가능한 아이템들의 가격 정보 초기화
    const QStringList &tradable = json.find("Tradable")->toVariant().toStringList();
    for (const QString &item : tradable)
        m_tradablePrice[item] = 0;
}

void ContentReward::initializeSearchOption()
{
    const QStringList &items = m_tradablePrice.keys();
    for (const QString &item : items)
    {
        SearchOption *pSearchOption = nullptr;

        if (item == "1레벨 멸화")
        {
            pSearchOption = new SearchOption(SearchType::Auction);
            pSearchOption->setCategoryCode(CategoryCode::Gem);
        }
        else
        {
            pSearchOption = new SearchOption(SearchType::Market);
            pSearchOption->setCategoryCode(CategoryCode::Reforge);
        }
        pSearchOption->setItemName(item);
        pSearchOption->setItemTier(3);
        pSearchOption->setPageNo(1);
        pSearchOption->setSortCondition("ASC");

        m_searchOptions << pSearchOption;
    }
}

void ContentReward::initializeContentSelector()
{
    QGroupBox *pGroupContentSelector = WidgetManager::createGroupBox("컨텐츠 선택");
    ui->hLayoutSelector->addWidget(pGroupContentSelector);
    m_widgets.append(pGroupContentSelector);

    QHBoxLayout *pLayoutContentSelector = new QHBoxLayout();
    pGroupContentSelector->setLayout(pLayoutContentSelector);
    m_layouts.append(pLayoutContentSelector);

    m_pContentSelector = WidgetManager::createComboBox(m_contents);
    pLayoutContentSelector->addWidget(m_pContentSelector);
    connect(m_pContentSelector, &QComboBox::currentIndexChanged, this, [&](int index){
        m_pSelectedTable->hide();
        m_pSelectedTable = m_rewardTables[index];
        m_pSelectedTable->show();
    });
}

void ContentReward::initializeTradableSelector()
{
    QGroupBox *pGroupTradableSelector = WidgetManager::createGroupBox("골드 가치에 포함");
    ui->hLayoutSelector->addWidget(pGroupTradableSelector);
    m_widgets.append(pGroupTradableSelector);

    QHBoxLayout *pLayoutTradableSelector = new QHBoxLayout();
    pLayoutTradableSelector->setSpacing(10);
    pGroupTradableSelector->setLayout(pLayoutTradableSelector);
    m_layouts.append(pLayoutTradableSelector);

    const QStringList items = {"명예의 파편", "파괴석", "수호석", "돌파석", "보석"};
    for (const QString &item : items)
    {
        QVBoxLayout *pLayoutCheckBox = new QVBoxLayout();
        pLayoutTradableSelector->addLayout(pLayoutCheckBox);
        m_layouts.append(pLayoutCheckBox);

        QLabel *pLabelCheckBox = WidgetManager::createLabel(item);
        pLayoutCheckBox->addWidget(pLabelCheckBox);
        m_widgets.append(pLabelCheckBox);

        QCheckBox *pCheckBox = new QCheckBox();
        pCheckBox->setChecked(true);
        pLayoutCheckBox->addWidget(pCheckBox);
        pLayoutCheckBox->setAlignment(pCheckBox, Qt::AlignHCenter);
        m_tradableSelector[item] = pCheckBox;
        connect(pCheckBox, &QCheckBox::stateChanged, this, [&](){
            for (ContentRewardTable *pRewardTable : m_rewardTables)
                pRewardTable->refreshTradablePrice(m_tradablePrice, m_tradableSelector);
        });
    }
}

void ContentReward::initializeRewardTable()
{
    for (const QString &content : m_contents)
    {
        ContentRewardTable *pContentRewardTable = new ContentRewardTable(content, m_contentLevels[content], m_dropTable);
        pContentRewardTable->hide();
        ui->vLayoutMain->addWidget(pContentRewardTable);
        m_rewardTables.append(pContentRewardTable);
    }
    m_pSelectedTable = m_rewardTables[0];
    m_pSelectedTable->show();
}

void ContentReward::initializeRewardAdder()
{
    m_pRewardAdder = new ContentRewardAdder(m_contents, m_contentLevels, m_dropTable);

    QPushButton *pButtonInsertData = WidgetManager::createPushButton("데이터 추가");
    ui->hLayoutSelector->addWidget(pButtonInsertData);
    m_widgets.append(pButtonInsertData);

    connect(pButtonInsertData, &QPushButton::released, this, [&](){
        if (!g_bAdmin)
        {
            QMessageBox msgBox;
            msgBox.setText("관리자 권한 필요");
            msgBox.exec();
            return;
        }

        m_pRewardAdder->show();
    });
}

void ContentReward::refreshRewardData()
{
    DbManager *pDbManager = DbManager::getInstance();
    QList<Collection> collections = {Collection::Reward_Chaos, Collection::Reward_Guardian};
    bsoncxx::builder::stream::document dummyFilter {};

    // reward data 초기화
    const QStringList &levelStages = m_rewardData.keys();
    for (const QString &levelStage : levelStages)
    {
        QString level = levelStage.back().isDigit() ? levelStage.chopped(1) : levelStage;
        m_rewardData[levelStage] = {0, QList<int>(m_dropTable[level].size(), 0)};
    }

    // db에 저장된 reward data 로드
    pDbManager->lock();
    for (const Collection &collection : collections)
    {
        QJsonArray data = pDbManager->findDocuments(collection, SortOrder::None, "", dummyFilter.extract());

        for (const QJsonValue &value : data)
        {
            const QJsonObject &object = value.toObject();
            const QString &level = object.find("Level")->toString();
            const QStringList &dropTable = level.back().isDigit() ? m_dropTable[level.chopped(1)] : m_dropTable[level];

            // reward data 추가
            RewardData newRewardData({0, QList<int>(dropTable.size(), 0)});
            newRewardData.dataCount = object.find("Count")->toInt();

            for (int i = 0; i < dropTable.size(); i++)
                newRewardData.itemCounts[i] = object.find(dropTable[i])->toInt();

            m_rewardData[level] += newRewardData;
        }
    }
    pDbManager->unlock();

    // RewardTable 업데이트
    for (ContentRewardTable *pRewardTable : m_rewardTables)
        pRewardTable->refreshRewardData(m_rewardData);
}

void ContentReward::refreshTradablePrice()
{
    m_tradableResponseCount = 0;

    const QStringList &items = m_tradablePrice.keys();
    for (int i = 0; i < items.size(); i++)
    {
        const QString &item = items[i];

        QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
        connect(pNetworkManager, &QNetworkAccessManager::finished, this, [this, item, items](QNetworkReply *pReply){
            m_tradableResponseCount++;

            QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
            if (response.isNull())
                return;

            const QJsonObject &object = response.object().find("Items")->toArray().at(0).toObject();

            // 가격 정보 parsing
            int minPrice = 0;

            if (item == "1레벨 멸화")
                minPrice = object.find("AuctionInfo")->toObject().find("BuyPrice")->toInt();
            else
                minPrice = object.find("CurrentMinPrice")->toInt();

            // 가격 update
            m_tradablePrice[item] = minPrice;

            // 모든 tradable의 가격 정보가 업데이트 되면 RewardTable에 반영
            if (m_tradableResponseCount == items.size())
            {
                for (ContentRewardTable *pRewardTable : m_rewardTables)
                    pRewardTable->refreshTradablePrice(m_tradablePrice, m_tradableSelector);
            }
        });
        connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

        SearchOption *pSearchOption = m_searchOptions[i];

        if (item == "1레벨 멸화")
            ApiManager::getInstance()->post(pNetworkManager, LostarkApi::Auction, QJsonDocument(pSearchOption->toJsonObject()).toJson());
        else
            ApiManager::getInstance()->post(pNetworkManager, LostarkApi::Market, QJsonDocument(pSearchOption->toJsonObject()).toJson());
    }
}

ContentReward *ContentReward::getInstance()
{
    if (m_pInstance == nullptr)
        m_pInstance = new ContentReward();

    return m_pInstance;
}

void ContentReward::destroyInstance()
{
    if (m_pInstance == nullptr)
        return;

    delete m_pInstance;
    m_pInstance = nullptr;
}

void ContentReward::start()
{
    refreshRewardData();
    refreshTradablePrice();
}
