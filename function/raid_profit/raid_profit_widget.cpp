#include "raid_profit_widget.h"
#include "ui_raid_profit_widget.h"
#include "ui/widget_manager.h"
#include "resource/resource_manager.h"
#include "api/lostark/item_manager.h"

#include <QLabel>
#include <QGroupBox>

RaidProfitWidget::RaidProfitWidget(const ContentReward &contentReward) :
    ui(new Ui::RaidProfitWidget)
{
    ui->setupUi(this);

    initializeContentLayout(contentReward.content);
    initializeRewardLayout(contentReward.rewards);
}

RaidProfitWidget::~RaidProfitWidget()
{
    delete ui;
}

void RaidProfitWidget::updateProfit(const QStringList &selectedItems)
{
    ItemManager *pItemManager = ItemManager::getInstance();

    for (int i = 0; i < mItemCountMap.size(); i++)
    {
        double totalPrice = 0;

        for (const QString &selectedItem : selectedItems)
        {
            if (selectedItem == "명예의 파편")
            {
                totalPrice +=
                    pItemManager->getMarketItem("명예의 파편 주머니(소)").minPrice /
                              static_cast<double>(500) *
                              mItemCountMap[i][selectedItem];
            }
            else if (selectedItem == "파괴석")
            {
                QString itemName;

                if (mItemCountMap[i].contains("파괴석 결정"))
                    itemName = "파괴석 결정";
                else if (mItemCountMap[i].contains("파괴강석"))
                    itemName = "파괴강석";
                else if (mItemCountMap[i].contains("정제된 파괴강석"))
                    itemName = "정제된 파괴강석";
                else
                    continue;

                totalPrice += pItemManager->getMarketItem(itemName).minPrice /
                              static_cast<double>(10) *
                              mItemCountMap[i][itemName];
            }
            else if (selectedItem == "수호석")
            {
                QString itemName;

                if (mItemCountMap[i].contains("수호석 결정"))
                    itemName = "수호석 결정";
                else if (mItemCountMap[i].contains("수호강석"))
                    itemName = "수호강석";
                else if (mItemCountMap[i].contains("정제된 수호강석"))
                    itemName = "정제된 수호강석";
                else
                    continue;

                totalPrice += pItemManager->getMarketItem(itemName).minPrice /
                              static_cast<double>(10) *
                              mItemCountMap[i][itemName];
            }
            else if (selectedItem == "돌파석")
            {
                QString itemName;

                if (mItemCountMap[i].contains("위대한 명예의 돌파석"))
                    itemName = "위대한 명예의 돌파석";
                else if (mItemCountMap[i].contains("경이로운 명예의 돌파석"))
                    itemName = "경이로운 명예의 돌파석";
                else if (mItemCountMap[i].contains("찬란한 명예의 돌파석"))
                    itemName = "찬란한 명예의 돌파석";
                else
                    continue;

                totalPrice += pItemManager->getMarketItem(itemName).minPrice *
                              mItemCountMap[i][itemName];
            }
            else if (selectedItem == "정수")
            {
                if (mItemCountMap[i].contains("선명한 지혜의 기운"))
                {
                    totalPrice +=
                        (pItemManager->getMarketItem("선명한 지혜의 정수").minPrice *
                            1.5 - 25) * mItemCountMap[i]["선명한 지혜의 기운"];
                }
                else if (mItemCountMap[i].contains("빛나는 지혜의 기운"))
                {
                    totalPrice +=
                        (pItemManager->getMarketItem("빛나는 지혜의 정수").minPrice *
                            1.5 - 62.5) * mItemCountMap[i]["빛나는 지혜의 기운"];
                }
            }
        }

        totalPrice -= mCosts[i];

        mProfitLabels[i]->setText(QString::number(totalPrice, 'f', 2));

        if (totalPrice < 0)
            mProfitLabels[i]->setStyleSheet("QLabel { color: red }");
        else if (totalPrice > 0)
            mProfitLabels[i]->setStyleSheet("QLabel { color: blue }");
        else
            mProfitLabels[i]->setStyleSheet("QLabel { color: black }");
    }
}

void RaidProfitWidget::initializeContentLayout(const QString &content)
{
    ui->vLayoutContent->addWidget(createContentLabel(content));
}

QLabel *RaidProfitWidget::createContentLabel(const QString &content)
{
    QLabel *pContentLabel = WidgetManager::createLabel(content, 16);
    pContentLabel->setStyleSheet("QLabel { color: white;"
                                 "         background-color: black; }");

    return pContentLabel;
}

void RaidProfitWidget::initializeRewardLayout(const QList<Reward> &rewards)
{
    for (const Reward &reward : rewards)
    {
        ui->vLayoutReward->addLayout(createLevelLayout(reward));
        ui->vLayoutReward->addWidget(WidgetManager::createLine(QFrame::HLine));
    }
}

QHBoxLayout *RaidProfitWidget::createLevelLayout(const Reward &reward)
{
    QHBoxLayout *pLevelLayout = new QHBoxLayout();

    pLevelLayout->addWidget(createGateLabel(reward.level));
    pLevelLayout->addWidget(createCostGroup(reward.cost));
    pLevelLayout->addWidget(createItemGroup(reward.items));
    pLevelLayout->addWidget(createProfitGroup());

    addItemCount(reward.items);
    mCosts << reward.cost;

    return pLevelLayout;
}

QLabel *RaidProfitWidget::createGateLabel(const QString &gate)
{
    QLabel *pGateLabel = WidgetManager::createLabel(gate, 14, 200);

    return pGateLabel;
}

QGroupBox *RaidProfitWidget::createCostGroup(const int cost)
{
    QGroupBox *pCostGroup = WidgetManager::createGroupBox("더보기 비용");
    pCostGroup->setMaximumWidth(200);

    QVBoxLayout *pCostGroupLayout = new QVBoxLayout();
    pCostGroup->setLayout(pCostGroupLayout);

    QLabel *pCostLabel = WidgetManager::createLabel(QString::number(cost));
    pCostGroupLayout->addWidget(pCostLabel);

    return pCostGroup;
}

QGroupBox *RaidProfitWidget::createItemGroup(const QList<RewardItem> &items)
{
    QGroupBox *pItemGroup = WidgetManager::createGroupBox("더보기 보상 목록");

    QHBoxLayout *pItemGroupLayout = new QHBoxLayout();
    pItemGroup->setLayout(pItemGroupLayout);

    for (const RewardItem &item : items)
        pItemGroupLayout->addLayout(createItemLayout(item));

    return pItemGroup;
}

QVBoxLayout *RaidProfitWidget::createItemLayout(const RewardItem &item)
{
    QVBoxLayout *pItemLayout = new QVBoxLayout();

    QLabel *pIcon = WidgetManager::createIcon(
        ResourceManager::getInstance()->iconPath(item.item), nullptr);
    pItemLayout->addWidget(pIcon, 0, Qt::AlignHCenter);

    QLabel *pItemCountLabel = WidgetManager::createLabel(QString::number(item.count));
    pItemLayout->addWidget(pItemCountLabel, 0, Qt::AlignHCenter);

    return pItemLayout;
}

QGroupBox *RaidProfitWidget::createProfitGroup()
{
    QGroupBox *pProfitGroup = WidgetManager::createGroupBox("더보기 손익");
    pProfitGroup->setMaximumWidth(200);

    QVBoxLayout *pProfitGroupLayout = new QVBoxLayout();
    pProfitGroup->setLayout(pProfitGroupLayout);

    pProfitGroupLayout->addWidget(createProfitLabel());

    return pProfitGroup;
}

QLabel *RaidProfitWidget::createProfitLabel()
{
    QLabel *pProfitLabel = WidgetManager::createLabel("-");

    mProfitLabels << pProfitLabel;

    return pProfitLabel;
}

void RaidProfitWidget::addItemCount(const QList<RewardItem> &items)
{
    QHash<QString, int> itemCountMap;

    for (const RewardItem &item : items)
        itemCountMap[item.item] = item.count;

    mItemCountMap << itemCountMap;
}
