#include "statistic_daily.h"
#include "ui_statistic_daily.h"
#include "ui/widget_manager.h"
#include "api/api_manager.h"
#include "api/lostark/item_manager.h"
#include "resource/resource_manager.h"
#include "function/statistic_daily/daily_data_adder.h"

#include <QLabel>
#include <QGroupBox>
#include <QCheckBox>
#include <QPushButton>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

StatisticDaily *StatisticDaily::pInstance = nullptr;

StatisticDaily::StatisticDaily() :
    ui(new Ui::StatisticDaily),
    mIsInitialized(false),
    mIsWorking(false)
{
    ui->setupUi(this);
    ui->vLayoutMain->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    initializeInfoLayout();
    initializeInputLayout();
    initializeResultLayout();
    initializeItemKeyMap();
}

StatisticDaily::~StatisticDaily()
{
    delete ui;
}

void StatisticDaily::initializeContentsData()
{
    mIsInitialized = true;
    mIsWorking = true;

    mContents = {"카오스던전", "가디언토벌"};

    for (const QString &content : mContents)
    {
        QNetworkAccessManager *pResourceLoader = new QNetworkAccessManager();

        connect(pResourceLoader, &QNetworkAccessManager::finished,
                pResourceLoader, &QNetworkAccessManager::deleteLater);
        connect(pResourceLoader, &QNetworkAccessManager::finished,
                this, [&, content](QNetworkReply *pReply)
        {
            if (!ApiManager::getInstance()->handleStatusCode(pReply))
                return;

            const QJsonArray &rewards = QJsonDocument::fromJson(pReply->readAll())
                                            .object().find("rewards")->toArray();

            for (auto rewardIter = rewards.constBegin(); rewardIter != rewards.constEnd(); rewardIter++)
            {
                const QString &level = rewardIter->toObject()
                                           .find("level")->toString();
                const QJsonArray &items = rewardIter->toObject()
                                              .find("items")->toArray();

                mContentLevels[content] << level;

                for (auto itemIter = items.constBegin(); itemIter != items.constEnd(); itemIter++)
                {
                    mLevelItemsMap[level] << itemIter->toObject()
                                                  .find("item")->toString();
                }
            }

            ui->vLayoutResult->addLayout(createContentLayout(content));

            loadStatisticData(content);
        });

        ApiManager::getInstance()->getResources(
            pResourceLoader, Resources::Reward, content);
    }
}

void StatisticDaily::initializeItemKeyMap()
{
    mItemKeyMap["실링"] = "silling";
    mItemKeyMap["명예의 파편"] = "shard";
    mItemKeyMap["파괴강석"] = "destructionStone";
    mItemKeyMap["정제된 파괴강석"] = "destructionStone";
    mItemKeyMap["수호강석"] = "protectionStone";
    mItemKeyMap["정제된 수호강석"] = "protectionStone";
    mItemKeyMap["경이로운 명예의 돌파석"] = "leapStone";
    mItemKeyMap["찬란한 명예의 돌파석"] = "leapStone";
    mItemKeyMap["보석"] = "gem";
}

void StatisticDaily::initializeInfoLayout()
{
    ui->hLayoutInfo->addWidget(createInfoLabel());
}

QLabel *StatisticDaily::createInfoLabel()
{
    const QString info = "휴식게이지 없이, 2수확 시 획득 가능한 보상 평균";

    return WidgetManager::createLabel(info, 16);
}

void StatisticDaily::initializeInputLayout()
{
    ui->hLayoutInput->setContentsMargins(0, 10, 0, 0);
    ui->hLayoutInput->setAlignment(Qt::AlignHCenter);
    ui->hLayoutInput->addWidget(createItemSelectorGroup());
}

QGroupBox *StatisticDaily::createItemSelectorGroup()
{
    QGroupBox *pGroupBox = WidgetManager::createGroupBox("골드에 포함시킬 아이템 선택");

    QHBoxLayout *pGroupLayout = new QHBoxLayout();
    pGroupLayout->setSpacing(15);
    pGroupBox->setLayout(pGroupLayout);

    QStringList itemNames = {"명예의 파편", "파괴석", "수호석", "돌파석", "보석"};
    QList<QStringList> itemKeys = {
        {"명예의 파편"},
        {"파괴강석", "정제된 파괴강석"},
        {"수호강석", "정제된 수호강석"},
        {"경이로운 명예의 돌파석", "찬란한 명예의 돌파석"},
        {"보석"}
    };

    for (int i = 0; i < itemNames.size(); i++) {
        pGroupLayout->addLayout(createItemSelector(itemNames[i], itemKeys[i]));
    }

    return pGroupBox;
}

QVBoxLayout *StatisticDaily::createItemSelector(const QString &itemName, const QStringList &keys)
{
    QVBoxLayout *pItemSelectorLayout = new QVBoxLayout();

    QLabel *pItemNameLabel = WidgetManager::createLabel(itemName);
    pItemSelectorLayout->addWidget(pItemNameLabel, 0, Qt::AlignHCenter);

    QCheckBox *pSelector = new QCheckBox();
    pSelector->setCheckState(Qt::Checked);
    pItemSelectorLayout->addWidget(pSelector, 0, Qt::AlignHCenter);

    connect(pSelector, &QCheckBox::stateChanged, this, [&]()
    {
        for (const QString &content : mContents)
            for (const QString &level : mContentLevels[content])
                updateItemPrice(level);
    });

    for (const QString &key : keys)
    {
        mItemSelectorMap[key] = pSelector;
    }

    return pItemSelectorLayout;
}

void StatisticDaily::initializeResultLayout()
{
    ui->vLayoutResult->setAlignment(Qt::AlignTop);
}

QVBoxLayout *StatisticDaily::createContentLayout(const QString &content)
{
    QVBoxLayout *pContentLayout = new QVBoxLayout();
    pContentLayout->addWidget(createContentLabel(content));

    for (const QString &level : mContentLevels[content])
    {
        pContentLayout->addLayout(createLevelLayout(content, level));
        pContentLayout->addWidget(WidgetManager::createLine(QFrame::HLine));
    }

    return pContentLayout;
}

QLabel *StatisticDaily::createContentLabel(const QString &content)
{
    QLabel *pContentLabel = WidgetManager::createLabel(content, 16);
    pContentLabel->setStyleSheet("QLabel { color: white;"
                                 "         background-color: black; }");

    return pContentLabel;
}

QHBoxLayout *StatisticDaily::createLevelLayout(const QString &content, const QString &level)
{
    QHBoxLayout *pLevelLayout = new QHBoxLayout();

    QLabel *pLevelLabel = WidgetManager::createLabel(level, 14);
    pLevelLayout->addWidget(pLevelLabel);

    const QStringList &items = mLevelItemsMap[level];

    for (const QString &item : items)
    {
        pLevelLayout->addLayout(createItemLayout(level, item));
    }

    pLevelLayout->addLayout(createItemLayout(level, "골드"));
    pLevelLayout->addLayout(createItemLayout(level, "데이터 수"));
    pLevelLayout->addWidget(createDataAddButton(content, level));

    return pLevelLayout;
}

QVBoxLayout *StatisticDaily::createItemLayout(const QString &level, const QString &itemName)
{
    QVBoxLayout *pItemLayout = new QVBoxLayout();

    const QString &iconPath = ResourceManager::getInstance()->iconPath(itemName);
    QLabel *pIcon = nullptr;

    if (iconPath == "")
        pIcon = WidgetManager::createLabel(itemName);
    else
        pIcon = WidgetManager::createIcon(iconPath, nullptr);

    pItemLayout->addWidget(pIcon, 0, Qt::AlignHCenter);

    QLabel *pItemCountLabel = WidgetManager::createLabel("-");
    pItemLayout->addWidget(pItemCountLabel, 0, Qt::AlignHCenter);

    mItemCountLabelMap[level][itemName] = pItemCountLabel;

    return pItemLayout;
}

QPushButton *StatisticDaily::createDataAddButton(const QString &content, const QString &level)
{
    QPushButton *pDataAddButton = WidgetManager::createPushButton("데이터 추가");

    connect(pDataAddButton, &QPushButton::released, this, [&, content, level]()
    {
        if (ApiManager::getInstance()->accessToken() == "")
        {
            QMessageBox msgBox;
            msgBox.setText("로그인이 필요합니다.");
            msgBox.exec();
            return;
        }
        else
        {
            DailyDataAdder *pDataAdder = new DailyDataAdder(content,
                                                            level,
                                                            mLevelItemsMap[level],
                                                            mItemKeyMap);
            pDataAdder->show();
            this->setDisabled(true);
        }
    });

    return pDataAddButton;
}

void StatisticDaily::loadStatisticData(const QString &content)
{
    const QStringList &levels = mContentLevels[content];
    const Statistics statistics = content == "카오스던전"
                                      ? Statistics::Chaos
                                      : Statistics::Guardian;

    for (const QString &level : levels)
    {
        QNetworkAccessManager *pStatisticLoader = new QNetworkAccessManager();

        connect(pStatisticLoader, &QNetworkAccessManager::finished,
                pStatisticLoader, &QNetworkAccessManager::deleteLater);
        connect(pStatisticLoader, &QNetworkAccessManager::finished,
                this, [&, level](QNetworkReply *pReply)
        {
            if (!ApiManager::getInstance()->handleStatusCode(pReply))
                return;

            updateItemCount(QJsonDocument::fromJson(pReply->readAll()).object());
            updateItemPrice(level);
        });

        ApiManager::getInstance()->getStatistics(pStatisticLoader, statistics, {level});
    }

    mIsWorking = false;
}

void StatisticDaily::updateItemCount(const QJsonObject &data)
{
    const QString &level = data.find("level")->toString();
    const QStringList &itemNames = mLevelItemsMap[level];
    const int dataCount = data.find("count")->toInt();

    mItemCountLabelMap[level]["데이터 수"]->setText(QString::number(dataCount));

    if (dataCount != 0)
    {
        for (const QString &itemName : itemNames)
        {
            int itemCount = data.find(mItemKeyMap[itemName])->toInt();
            double avgCount = itemCount / static_cast<double>(dataCount);

            mItemCountMap[level][itemName] = avgCount;
            mItemCountLabelMap[level][itemName]->setText(
                QString::number(avgCount, 'f', 2));
        }
    }
}

void StatisticDaily::updateItemPrice(const QString &level)
{
    const QStringList &itemNames = mLevelItemsMap[level];

    ItemManager *pItemManager = ItemManager::getInstance();
    double totalPrice = 0;

    for (const QString &itemName : itemNames)
    {
        double price = 0;

        if (!mItemSelectorMap.contains(itemName) ||
            !mItemSelectorMap.value(itemName)->isChecked())
            continue;

        if (itemName == "명예의 파편")
            price = pItemManager->getMarketItem("명예의 파편 주머니(소)")
                        .minPrice / static_cast<double>(500);
        else if (itemName.contains("강석"))
            price = pItemManager->getMarketItem(itemName)
                        .minPrice / static_cast<double>(10);
        else if (itemName == "보석")
            price = pItemManager->getAuctionItem("1레벨 멸화의 보석").buyPrice;
        else if (itemName.contains("돌파석"))
            price = pItemManager->getMarketItem(itemName).minPrice;

        totalPrice += mItemCountMap[level][itemName] * price;
    }

    mItemCountLabelMap[level]["골드"]->setText(QString::number(totalPrice, 'f', 2));
}

void StatisticDaily::refresh()
{
    if (!mIsInitialized)
        initializeContentsData();
    else
    {
        for (const QString &content : mContents)
        {
            loadStatisticData(content);
        }
    }
}

StatisticDaily *StatisticDaily::getInstance()
{
    if (pInstance == nullptr)
        pInstance = new StatisticDaily();

    return pInstance;
}

void StatisticDaily::destroyInstance()
{
    if (pInstance == nullptr)
        return;

    delete pInstance;
    pInstance = nullptr;
}
