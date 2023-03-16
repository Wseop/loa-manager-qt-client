#include "content_reward.h"
#include "ui_content_reward.h"
#include "ui/widget_manager.h"
#include "resource/resource_manager.h"
#include "api/response_parser.h"
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

extern bool gbAdmin;

ContentReward *ContentReward::m_pInstance = nullptr;

ContentReward::ContentReward() :
    ui(new Ui::ContentReward),
    mpContentSelector(nullptr),
    mpSelectedTable(nullptr),
    mpRewardAdder(nullptr)
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
    for (SearchOption *pSearchOption : mSearchOptions)
        delete pSearchOption;
    mSearchOptions.clear();

    if (mpContentSelector != nullptr)
    {
        delete mpContentSelector;
        mpContentSelector = nullptr;
    }

    for (auto iter = mTradableSelector.begin(); iter != mTradableSelector.end(); iter++)
        delete iter.value();
    mTradableSelector.clear();

    for (ContentRewardTable *pRewardTable : mRewardTables)
        delete pRewardTable;
    mRewardTables.clear();

    if (mpRewardAdder != nullptr)
        delete mpRewardAdder;
    mpRewardAdder = nullptr;

    for (QWidget *pWidget : mWidgets)
        delete pWidget;
    mWidgets.clear();

    for (QLayout *pLayout : mLayouts)
        delete pLayout;
    mLayouts.clear();

    delete ui;
}

void ContentReward::initializeContentData()
{
    QJsonObject json = ResourceManager::getInstance()->loadJson("drop_table");

    // 컨텐츠 목록 초기화
    mContents = json.find("Contents")->toVariant().toStringList();

    // 컨텐츠별 data 초기화
    for (const QString &content : mContents)
    {
        const QJsonArray &dropTable = json.find(content)->toObject().find("DropTable")->toArray();

        for (const QJsonValue &value : dropTable)
        {
            const QJsonObject &object = value.toObject();
            const QString &level = object.find("Level")->toString();
            int stage = object.find("Stage")->toInt();

            mContentLevels[content] << level;
            mDropTable[level] = object.find("ItemList")->toVariant().toStringList();

            for (int i = 0; i < stage; i++)
            {
                const QString levelStage = content == "카오스 던전" ? level + QString::number(i + 1) : level;
                mRewardData[levelStage] = {0, QList<int>(mDropTable[level].size(), 0)};
            }
        }
    }

    // 골드로 환산이 가능한 아이템들의 가격 정보 초기화
    const QStringList &tradable = json.find("Tradable")->toVariant().toStringList();
    for (const QString &item : tradable)
        mTradablePrice[item] = 0;
}

void ContentReward::initializeSearchOption()
{
    const QStringList &items = mTradablePrice.keys();
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

        mSearchOptions << pSearchOption;
    }
}

void ContentReward::initializeContentSelector()
{
    QGroupBox *pGroupContentSelector = WidgetManager::createGroupBox("컨텐츠 선택");
    ui->hLayoutSelector->addWidget(pGroupContentSelector);
    mWidgets.append(pGroupContentSelector);

    QHBoxLayout *pLayoutContentSelector = new QHBoxLayout();
    pGroupContentSelector->setLayout(pLayoutContentSelector);
    mLayouts.append(pLayoutContentSelector);

    mpContentSelector = WidgetManager::createComboBox(mContents);
    pLayoutContentSelector->addWidget(mpContentSelector);
    connect(mpContentSelector, &QComboBox::currentIndexChanged, this, [&](int index){
        mpSelectedTable->hide();
        mpSelectedTable = mRewardTables[index];
        mpSelectedTable->show();
    });
}

void ContentReward::initializeTradableSelector()
{
    QGroupBox *pGroupTradableSelector = WidgetManager::createGroupBox("골드 가치에 포함");
    ui->hLayoutSelector->addWidget(pGroupTradableSelector);
    mWidgets.append(pGroupTradableSelector);

    QHBoxLayout *pLayoutTradableSelector = new QHBoxLayout();
    pLayoutTradableSelector->setSpacing(10);
    pGroupTradableSelector->setLayout(pLayoutTradableSelector);
    mLayouts.append(pLayoutTradableSelector);

    const QStringList items = {"명예의 파편", "파괴석", "수호석", "돌파석", "보석"};
    for (const QString &item : items)
    {
        QVBoxLayout *pLayoutCheckBox = new QVBoxLayout();
        pLayoutTradableSelector->addLayout(pLayoutCheckBox);
        mLayouts.append(pLayoutCheckBox);

        QLabel *pLabelCheckBox = WidgetManager::createLabel(item);
        pLayoutCheckBox->addWidget(pLabelCheckBox);
        mWidgets.append(pLabelCheckBox);

        QCheckBox *pCheckBox = new QCheckBox();
        pCheckBox->setChecked(true);
        pLayoutCheckBox->addWidget(pCheckBox);
        pLayoutCheckBox->setAlignment(pCheckBox, Qt::AlignHCenter);
        mTradableSelector[item] = pCheckBox;
        connect(pCheckBox, &QCheckBox::stateChanged, this, [&](){
            for (ContentRewardTable *pRewardTable : mRewardTables)
                pRewardTable->refreshTradablePrice(mTradablePrice, mTradableSelector);
        });
    }
}

void ContentReward::initializeRewardTable()
{
    for (const QString &content : mContents)
    {
        ContentRewardTable *pContentRewardTable = new ContentRewardTable(content, mContentLevels[content], mDropTable);
        pContentRewardTable->hide();
        ui->vLayoutMain->addWidget(pContentRewardTable);
        mRewardTables.append(pContentRewardTable);
    }
    mpSelectedTable = mRewardTables[0];
    mpSelectedTable->show();
}

void ContentReward::initializeRewardAdder()
{
    mpRewardAdder = new ContentRewardAdder(mContents, mContentLevels, mDropTable);

    QPushButton *pButtonInsertData = WidgetManager::createPushButton("데이터 추가");
    ui->hLayoutSelector->addWidget(pButtonInsertData);
    mWidgets.append(pButtonInsertData);

    connect(pButtonInsertData, &QPushButton::released, this, [&](){
        if (!gbAdmin)
        {
            QMessageBox msgBox;
            msgBox.setText("관리자 권한 필요");
            msgBox.exec();
            return;
        }

        mpRewardAdder->show();
    });
}

void ContentReward::refreshRewardData()
{
    DbManager *pDbManager = DbManager::getInstance();
    QList<Collection> collections = {Collection::Reward_Chaos, Collection::Reward_Guardian};
    bsoncxx::builder::stream::document dummyFilter {};

    // reward data 초기화
    const QStringList &levelStages = mRewardData.keys();
    for (const QString &levelStage : levelStages)
    {
        QString level = levelStage.back().isDigit() ? levelStage.chopped(1) : levelStage;
        mRewardData[levelStage] = {0, QList<int>(mDropTable[level].size(), 0)};
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
            const QStringList &dropTable = level.back().isDigit() ? mDropTable[level.chopped(1)] : mDropTable[level];

            // reward data 추가
            RewardData newRewardData({0, QList<int>(dropTable.size(), 0)});
            newRewardData.dataCount = object.find("Count")->toInt();

            for (int i = 0; i < dropTable.size(); i++)
                newRewardData.itemCounts[i] = object.find(dropTable[i])->toInt();

            mRewardData[level] += newRewardData;
        }
    }
    pDbManager->unlock();

    // RewardTable 업데이트
    for (ContentRewardTable *pRewardTable : mRewardTables)
        pRewardTable->refreshRewardData(mRewardData);
}

void ContentReward::refreshTradablePrice()
{
    mTradableResponseCount = 0;

    const QStringList &items = mTradablePrice.keys();
    for (int i = 0; i < items.size(); i++)
    {
        const QString &item = items[i];

        QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
        connect(pNetworkManager, &QNetworkAccessManager::finished, this, [this, item, items](QNetworkReply *pReply){
            mTradableResponseCount++;

            QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
            if (response.isNull())
                return;

            // 가격 정보 parsing
            int minPrice = 0;

            if (item == "1레벨 멸화")
            {
                ResponseAuction responseAuction = ResponseParser::parseAuctionItem(response);
                minPrice = responseAuction.items.front().AuctionInfo.buyPrice;
            }
            else
            {
                ResponseMarket responseMarket = ResponseParser::parseMarketItem(response);
                minPrice = responseMarket.items.front().currentMinPrice;
            }

            // 가격 update
            mTradablePrice[item] = minPrice;

            // 모든 tradable의 가격 정보가 업데이트 되면 RewardTable에 반영
            if (mTradableResponseCount == items.size())
            {
                for (ContentRewardTable *pRewardTable : mRewardTables)
                    pRewardTable->refreshTradablePrice(mTradablePrice, mTradableSelector);
            }
        });
        connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

        SearchOption *pSearchOption = mSearchOptions[i];

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
