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
    initializeProfitTable();
}

RaidProfit::~RaidProfit()
{
    for (SearchOption *pSearchOption : mSearchOptions)
        delete pSearchOption;
    mSearchOptions.clear();

    for (QWidget *pWidget : mWidgets)
        delete pWidget;
    mWidgets.clear();

    for (QLayout *pLayout : mLayouts)
        delete pLayout;
    mLayouts.clear();

    delete ui;
}

void RaidProfit::initializeRaidReward()
{
    QJsonObject json = ResourceManager::getInstance()->loadJson("raid_reward");

    // 전체 콘텐츠 목록 초기화
    mContents = json.find("Contents")->toVariant().toStringList();

    // 아이템 가격 테이블 초기화
    const QStringList &rewards = json.find("Rewards")->toVariant().toStringList();
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
    for (const QString &content : mContents)
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

        mCosts << costsPerRaid;
        mRewardItems << rewardItemsPerRaid;
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
    mWidgets << pGroupContentSelect;

    QHBoxLayout *pLayoutContentSelect = new QHBoxLayout();
    pGroupContentSelect->setLayout(pLayoutContentSelect);
    mLayouts << pLayoutContentSelect;

    mpContentSelector = WidgetManager::createComboBox(mContents);
    pLayoutContentSelect->addWidget(mpContentSelector);
    mWidgets << mpContentSelector;

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
    mWidgets << pGroupItemSelect;

    QHBoxLayout *pLayoutItemSelect = new QHBoxLayout();
    pGroupItemSelect->setLayout(pLayoutItemSelect);
    mLayouts << pLayoutItemSelect;

    const QStringList items = {"명예의 파편", "파괴석", "수호석", "돌파석", "정수"};
    for (const QString &item : items)
    {
        QVBoxLayout *pLayout = new QVBoxLayout();
        pLayoutItemSelect->addLayout(pLayout);
        mLayouts << pLayout;

        QLabel *pLabel = WidgetManager::createLabel(item);
        pLayout->addWidget(pLabel);
        mWidgets << pLabel;

        QCheckBox *pItemSelector = new QCheckBox();
        pItemSelector->setChecked(true);
        pLayout->addWidget(pItemSelector);
        pLayout->setAlignment(pItemSelector, Qt::AlignHCenter);
        mItemSelectors << pItemSelector;
        mWidgets << pItemSelector;

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
        mWidgets << pProfitTable;
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
                const QJsonObject &item = response.object().find("Items")->toArray().first().toObject();
                const QString &itemName = item.find("Name")->toString();
                int itemPrice = item.find("CurrentMinPrice")->toInt();

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

        ApiManager::getInstance()->post(pNetworkManager, LostarkApi::Market, QJsonDocument(mSearchOptions[i]->toJsonObject()).toJson());
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
