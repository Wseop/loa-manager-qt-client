#include "content_reward.h"
#include "ui_content_reward.h"
#include "ui/widget_manager.h"
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
}

ContentReward::~ContentReward()
{
    delete ui;
}

void ContentReward::initializeContentData()
{
    // 컨텐츠 목록 초기화
    mContents = {"카오스던전", "가디언토벌"};

    // 골드로 환산이 가능한 아이템들의 가격 정보 초기화
    const QStringList &tradable = {
        "명예의 파편 주머니(소)",
        "파괴강석", "정제된 파괴강석",
        "수호강석", "정제된 수호강석",
        "경이로운 명예의 돌파석", "찬란한 명예의 돌파석",
        "1레벨 멸화"
    };

    for (const QString &item : tradable)
    {
        mTradablePrice[item] = 0;
    }

    // 컨텐츠별 data 초기화
    for (int i = 0; i < mContents.size(); i++) {
        QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
        connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&, i](QNetworkReply *pReply){
            QJsonArray rewards = QJsonDocument::fromJson(pReply->readAll())
                                    .object()
                                    .find("rewards")->toArray();

            for (const QJsonValue &value : rewards) {
                const QJsonObject &reward = value.toObject();
                const QString &level = reward.find("level")->toString();
                const QJsonArray items = reward.find("items")->toArray();

                mContentLevels[mContents[i]] << level;

                for (const QJsonValue &value : items) {
                    mDropTable[level] << value.toObject().find("item")->toString();
                }

                mRewardData[level] = {level, 0, {}, QList<int>(mDropTable[level].size(), 0)};

                mTotalLevels++;
            }

            mInitStatus |= (1 << i);

            if (mInitStatus == 0x03) {
                initializeRewardTable();
                initializeRewardAdder();
            }
        });
        connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

        ApiManager::getInstance()->get(pNetworkManager,
                                       ApiType::LoaManager,
                                       static_cast<int>(LoamanagerApi::GetResource),
                                       {"reward", mContents[i]},
                                       "");
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

    QHBoxLayout *pLayoutContentSelector = new QHBoxLayout();
    pGroupContentSelector->setLayout(pLayoutContentSelector);

    mpContentSelector = WidgetManager::createComboBox(mContents);
    pLayoutContentSelector->addWidget(mpContentSelector);

    connect(mpContentSelector, &QComboBox::currentIndexChanged, this, [&](int index)
    {
        mpSelectedTable->hide();
        mpSelectedTable = mRewardTables[index];
        mpSelectedTable->show();
    });
}

void ContentReward::initializeTradableSelector()
{
    QGroupBox *pGroupTradableSelector = WidgetManager::createGroupBox("골드 가치에 포함");
    ui->hLayoutSelector->addWidget(pGroupTradableSelector);

    QHBoxLayout *pLayoutTradableSelector = new QHBoxLayout();
    pLayoutTradableSelector->setSpacing(10);
    pGroupTradableSelector->setLayout(pLayoutTradableSelector);

    const QStringList items = {"명예의 파편", "파괴석", "수호석", "돌파석", "보석"};

    for (const QString &item : items)
    {
        QVBoxLayout *pLayoutCheckBox = new QVBoxLayout();
        pLayoutTradableSelector->addLayout(pLayoutCheckBox);

        QLabel *pLabelCheckBox = WidgetManager::createLabel(item);
        pLayoutCheckBox->addWidget(pLabelCheckBox);

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

    connect(pButtonInsertData, &QPushButton::released, this, [&]()
    {
        const QString &accessToken = ApiManager::getInstance()->accessToken();

        if (accessToken == "") {
            QMessageBox msgBox;
            msgBox.setText("로그인이 필요합니다.");
            msgBox.exec();
        } else {
            mpRewardAdder->show();
        }
    });
}

void ContentReward::refreshRewardData()
{
    // reward data 초기화
    const QStringList &levels = mRewardData.keys();

    for (const QString &level : levels)
    {
        mRewardData[level] = {level, 0, {}, QList<int>(mDropTable[level].size(), 0)};
    }

    mResponseCount = 0;

    // reward data 로드
    for (const QString &content : mContents)
    {
        for (const QString &contentLevel : mContentLevels[content])
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
                    mRewardData[reward.level] += reward;
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

            QString category = content == "카오스던전" ? "chaos": "guardian";

            ApiManager::getInstance()->get(pNetworkManager,
                                           ApiType::LoaManager,
                                           static_cast<int>(LoamanagerApi::GetStats),
                                           {category, contentLevel},
                                           "");
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
                minPrice = responseAuction.items.front().auctionInfo.buyPrice;
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
            ApiManager::getInstance()->post(pNetworkManager,
                                            ApiType::Lostark,
                                            static_cast<int>(LostarkApi::Auction),
                                            {},
                                            QJsonDocument(pSearchOption->toJsonObject()).toJson());
        else
            ApiManager::getInstance()->post(pNetworkManager,
                                            ApiType::Lostark,
                                            static_cast<int>(LostarkApi::Market),
                                            {},
                                            QJsonDocument(pSearchOption->toJsonObject()).toJson());
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
