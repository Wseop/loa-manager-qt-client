#include "raid_profit.h"
#include "ui_raid_profit.h"
#include "resource/resource_manager.h"
#include "api/response_parser.h"
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

RaidProfit *RaidProfit::mpInstance = nullptr;

RaidProfit::RaidProfit() :
    ui(new Ui::RaidProfit),
    mResponseCount(0),
    mpContentSelector(nullptr),
    mpCurrentTable(nullptr)
{
    ui->setupUi(this);
    ui->hLayoutSelector->setAlignment(Qt::AlignHCenter);

    initializeRaidReward();
    initializeSearchOption();
    initializeContentSelect();
    initializeItemSelect();
}

RaidProfit::~RaidProfit()
{
    delete ui;
}

void RaidProfit::initializeRaidReward()
{
    // 전체 콘텐츠 목록 초기화
    mContents = {"발탄(노말)", "발탄(하드)",
                 "비아키스(노말)", "비아키스(하드)",
                 "쿠크세이튼",
                 "아브렐슈드(노말)", "아브렐슈드(하드)",
                 "카양겔(노말)", "카양겔(하드)",
                 "일리아칸(노말)", "일리아칸(하드)",
                 "혼돈의 상아탑(노말)", "혼돈의 상아탑(하드)"};

    // 아이템 가격 테이블 초기화
    const QStringList &rewards = {
        "명예의 파편 주머니(소)", "파괴석 결정", "파괴강석", "정제된 파괴강석", "수호석 결정", "수호강석", "정제된 수호강석",
        "위대한 명예의 돌파석", "경이로운 명예의 돌파석", "찬란한 명예의 돌파석", "선명한 지혜의 정수", "빛나는 지혜의 정수"
    };

    for (const QString &reward : rewards)
    {
        if (reward.contains("명예의 파편"))
            mItemPrice["명예의 파편"] = 0;
        else if (reward == "선명한 지혜의 정수")
            mItemPrice["선명한 지혜의 기운"] = 0;
        else if (reward == "빛나는 지혜의 정수")
            mItemPrice["빛나는 지혜의 기운"] = 0;
        else
            mItemPrice[reward] = 0;
    }

    // 레이드 관문별 더보기 보상 정보 초기화
    mCosts = QList<QList<int>>(mContents.size());
    mRewardItems = QList<QList<RewardItems>>(mContents.size());

    for (int i = 0; i < mContents.size(); i++)
    {
        QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
        connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&, i](QNetworkReply *pReply){
            QJsonArray rewards = QJsonDocument::fromJson(pReply->readAll()).object()
                                                                           .find("rewards")->toArray();

            QList<int> costsPerRaid;
            QList<RewardItems> rewardItemsPerRaid;

            for (const QJsonValue &value : rewards)
            {
                const QJsonObject &reward = value.toObject();
                const QJsonArray &items = reward.find("items")->toArray();
                RewardItems rewardItemsPerGate;

                costsPerRaid << reward.find("cost")->toInt();

                for (const QJsonValue &value : items)
                {
                    const QJsonObject &item = value.toObject();
                    const QString &name = item.find("item")->toString();
                    int count = item.find("count")->toInt();

                    rewardItemsPerGate.append({name, count});
                }

                rewardItemsPerRaid << rewardItemsPerGate;
            }

            mCosts[i] = costsPerRaid;
            mRewardItems[i] = rewardItemsPerRaid;

            mInitStatus |= (1 << i);

            if (mInitStatus == (1 << mContents.size()) - 1) {
                initializeProfitTable();
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

void RaidProfit::initializeSearchOption()
{
    // Item 가격 검색 시 필요한 SearchOption 초기화
    const QStringList &items = mItemPrice.keys();
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

        mSearchOptions << pSearchOption;
    }
}

void RaidProfit::initializeContentSelect()
{
    QGroupBox *pGroupContentSelect = WidgetManager::createGroupBox("레이드 선택");
    ui->hLayoutSelector->addWidget(pGroupContentSelect);

    QHBoxLayout *pLayoutContentSelect = new QHBoxLayout();
    pGroupContentSelect->setLayout(pLayoutContentSelect);

    mpContentSelector = WidgetManager::createComboBox(mContents);
    pLayoutContentSelect->addWidget(mpContentSelector);

    connect(mpContentSelector, &QComboBox::currentIndexChanged, this, [&](int index){
        mpCurrentTable->hide();
        mpCurrentTable = mProfitTables[index];
        mpCurrentTable->show();
    });
}

void RaidProfit::initializeItemSelect()
{
    QGroupBox *pGroupItemSelect = WidgetManager::createGroupBox("손익 계산에 포함");
    ui->hLayoutSelector->addWidget(pGroupItemSelect);

    QHBoxLayout *pLayoutItemSelect = new QHBoxLayout();
    pGroupItemSelect->setLayout(pLayoutItemSelect);

    const QStringList items = {"명예의 파편", "파괴석", "수호석", "돌파석", "정수"};
    for (const QString &item : items)
    {
        QVBoxLayout *pLayout = new QVBoxLayout();
        pLayoutItemSelect->addLayout(pLayout);

        QLabel *pLabel = WidgetManager::createLabel(item);
        pLayout->addWidget(pLabel);

        QCheckBox *pItemSelector = new QCheckBox();
        pItemSelector->setChecked(true);
        pLayout->addWidget(pItemSelector);
        pLayout->setAlignment(pItemSelector, Qt::AlignHCenter);
        mItemSelectors << pItemSelector;

        connect(pItemSelector, &QCheckBox::stateChanged, this, [&](){
            for (RaidProfitTable *pProfitTable : mProfitTables)
                pProfitTable->refreshItemPrice(mItemPrice, mItemSelectors);
        });
    }
}

void RaidProfit::initializeProfitTable()
{
    for (int i = 0; i < mContents.size(); i++)
    {
        RaidProfitTable *pProfitTable = new RaidProfitTable(mContents[i], mCosts[i], mRewardItems[i]);
        pProfitTable->hide();
        ui->vLayoutMain->addWidget(pProfitTable);

        mProfitTables << pProfitTable;
    }

    mpCurrentTable = mProfitTables[0];
    mpCurrentTable->show();
}

void RaidProfit::refreshItemPrice()
{
    mResponseCount = 0;

    for (int i = 0; i < mItemPrice.size(); i++)
    {
        QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
        connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&](QNetworkReply *pReply){
            mResponseCount++;

            // 아이템 가격 검색 결과 처리
            QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
            if (!response.isNull())
            {
                ResponseMarket responseMarket = ResponseParser::parseMarketItem(response);
                const QString &itemName = responseMarket.items.front().name;
                int itemPrice = responseMarket.items.front().currentMinPrice;

                if (itemName.contains("명예의 파편"))
                    mItemPrice["명예의 파편"] = itemPrice / (double)500;
                else if (itemName == "선명한 지혜의 정수")
                    mItemPrice["선명한 지혜의 기운"] = itemPrice * 1.5;
                else if (itemName == "빛나는 지혜의 정수")
                    mItemPrice["빛나는 지혜의 기운"] = itemPrice * 1.5;
                else if (itemName.contains("파괴") || itemName.contains("수호"))
                    mItemPrice[itemName] = itemPrice / (double)10;
                else
                    mItemPrice[itemName] = itemPrice;
            }

            // 모든 아이템 검색 완료 시 UI Update
            if (mResponseCount == mItemPrice.size())
            {
                for (RaidProfitTable *pProfitTable : mProfitTables)
                    pProfitTable->refreshItemPrice(mItemPrice, mItemSelectors);
            }
        });
        connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

        ApiManager::getInstance()->post(pNetworkManager,
                                        ApiType::Lostark,
                                        static_cast<int>(LostarkApi::Market),
                                        {},
                                        QJsonDocument(mSearchOptions[i]->toJsonObject()).toJson());
    }
}

RaidProfit *RaidProfit::getInstance()
{
    if (mpInstance == nullptr)
        mpInstance = new RaidProfit();

    return mpInstance;
}

void RaidProfit::destroyInstance()
{
    if (mpInstance == nullptr)
        return;

    delete mpInstance;
    mpInstance = nullptr;
}

void RaidProfit::start()
{
    refreshItemPrice();
}
