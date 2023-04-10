#include "content_reward.h"
#include "ui_content_reward.h"
#include "ui/widget_manager.h"
#include "resource/resource_manager.h"
#include "api/api_manager.h"
#include "api/response_parser.h"
#include "api/lostark/search_option.h"
#include "api/loamanager/loamanager_api.h"
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
    mTotalLevels(0),
    mResponseCount(0),
    mpContentSelector(nullptr),
    mpSelectedTable(nullptr),
    mpRewardAdder(nullptr),
    mpLabelInfo(nullptr)
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

    for (auto rIter = mLayouts.rbegin(); rIter != mLayouts.rend(); rIter++)
        delete *rIter;
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

            mContentLevels[content] << level;
            mDropTable[level] = object.find("ItemList")->toVariant().toStringList();
            mRewardData[level] = {0, QList<int>(mDropTable[level].size(), 0)};

            mTotalLevels++;
        }
    }

    // 골드로 환산이 가능한 아이템들의 가격 정보 초기화
    const QStringList &tradable = json.find("Tradable")->toVariant().toStringList();

    for (const QString &item : tradable)
    {
        mTradablePrice[item] = 0;
    }
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
        connect(pCheckBox, &QCheckBox::stateChanged, this, [&]()
        {
            for (ContentRewardTable *pRewardTable : mRewardTables)
            {
                pRewardTable->refreshTradablePrice(mTradablePrice, mTradableSelector);
            }
        });
    }
}

void ContentReward::initializeRewardTable()
{
    // 로딩 메세지
    mpLabelInfo = WidgetManager::createLabel("데이터 불러오는 중...", 16);
    ui->vLayoutMain->addWidget(mpLabelInfo);
    ui->vLayoutMain->setAlignment(mpLabelInfo, Qt::AlignCenter);

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

    connect(pButtonInsertData, &QPushButton::released, this, [&]()
    {
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
    // reward data 초기화
    const QStringList &levels = mRewardData.keys();

    for (const QString &level : levels)
    {
        mRewardData[level] = {0, QList<int>(mDropTable[level].size(), 0)};
    }

    mResponseCount = 0;

    // reward data 로드
    for (const QString &content : mContents)
    {
        const QStringList &contentLevels = mContentLevels[content];

        for (const QString &contentLevel : contentLevels)
        {
            QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();

            connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&](QNetworkReply *pReply)
            {
                mResponseCount++;

                QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());

                if (response.isNull())
                    return;

                QList<Reward> rewards = ResponseParser::parseRewards(response);

                for (const Reward &reward : rewards)
                {
                    RewardData rewardData {reward.count, reward.itemCounts};

                    mRewardData[reward.level] += rewardData;
                }

                if (mResponseCount == mTotalLevels)
                {
                    for (ContentRewardTable *pRewardTable : mRewardTables)
                    {
                        pRewardTable->refreshRewardData(mRewardData);
                    }

                    refreshTradablePrice();
                }
            });
            connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

            LoamanagerApi api = content == "카오스 던전" ? LoamanagerApi::GetRewardChaos : LoamanagerApi::GetRewardGuardian;

            ApiManager::getInstance()->get(pNetworkManager, ApiType::LoaManager, static_cast<int>(api), contentLevel, "");
        }
    }
}

void ContentReward::refreshTradablePrice()
{
    mResponseCount = 0;

    const QStringList &items = mTradablePrice.keys();
    for (int i = 0; i < items.size(); i++)
    {
        const QString &item = items[i];

        QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
        connect(pNetworkManager, &QNetworkAccessManager::finished, this, [this, item, items](QNetworkReply *pReply)
        {
            mResponseCount++;

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
            if (mResponseCount == items.size())
            {
                for (ContentRewardTable *pRewardTable : mRewardTables)
                {
                    pRewardTable->refreshTradablePrice(mTradablePrice, mTradableSelector);
                }
            }

            // 로딩 메세지 제거
            delete mpLabelInfo;
            mpLabelInfo = nullptr;
        });
        connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

        SearchOption *pSearchOption = mSearchOptions[i];

        if (item == "1레벨 멸화")
            ApiManager::getInstance()->post(pNetworkManager, ApiType::Lostark, static_cast<int>(LostarkApi::Auction), QJsonDocument(pSearchOption->toJsonObject()).toJson());
        else
            ApiManager::getInstance()->post(pNetworkManager, ApiType::Lostark, static_cast<int>(LostarkApi::Market), QJsonDocument(pSearchOption->toJsonObject()).toJson());
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
}
