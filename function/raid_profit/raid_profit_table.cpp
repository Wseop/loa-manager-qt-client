#include "raid_profit_table.h"
#include "ui_raid_profit_table.h"
#include "ui/widget_manager.h"
#include "resource/resource_manager.h"

#include <QLabel>
#include <QFrame>
#include <QCheckBox>

RaidProfitTable::RaidProfitTable(const QString &content, const QList<int> &costs, const QList<RewardItems> &rewardItems) :
    ui(new Ui::RaidProfitTable),
    mContent(content),
    mCosts(costs),
    mRewardItems(rewardItems)
{
    ui->setupUi(this);

    initializeTable();
}

RaidProfitTable::~RaidProfitTable()
{
    for (QWidget *pWidget : mWidgets)
        delete pWidget;
    mWidgets.clear();

    for (auto rIter = mLayouts.rbegin(); rIter != mLayouts.rend(); rIter++)
        delete *rIter;
    mLayouts.clear();

    delete ui;
}

void RaidProfitTable::refreshItemPrice(const QHash<QString, double> &itemPrice, const QList<QCheckBox*> &itemSelectors)
{
    for (int i = 0; i < mCosts.size(); i++)
    {
        const RewardItems &rewardItems = mRewardItems[i];
        double totalGold = 0;
        double profit = 0;

        for (int j = 0; j < rewardItems.size(); j++)
        {
            const QString &itemName = rewardItems[j].first;
            int itemCount = rewardItems[j].second;
            double price = itemPrice[itemName];

            if (itemSelectors[j]->isChecked())
                totalGold += price * itemCount;
        }

        mGoldLabels[i]->setText(QString("%L1").arg(totalGold, 0, 'f', 2));

        profit = totalGold - mCosts[i];
        mProfitLabels[i]->setText(QString("%L1").arg(profit, 0, 'f', 2));

        if (profit > 0)
            mProfitLabels[i]->setStyleSheet("QLabel { color: blue }");
        else
            mProfitLabels[i]->setStyleSheet("QLabel { color: red }");
    }
}

void RaidProfitTable::initializeTable()
{
    QList<int> colSpans = {1, static_cast<int>(mRewardItems[0].size()), 1, 1, 1};

    initializeTableColumn(colSpans);
    initializeTableRow(colSpans);
    initializeTableReward();
}

void RaidProfitTable::initializeTableColumn(const QList<int> &colSpans)
{
    int col = 0;

    const QStringList attributes = {"관문", "더보기 보상 목록", "더보기 비용", "골드 가치 합계", "더보기 손익"};
    for (int i = 0; i < attributes.size(); i++)
    {
        QLabel *pLabel = WidgetManager::createLabel(attributes[i], 12, "", 150);
        ui->gridTable->addWidget(pLabel, 0, col, 1, colSpans[i], Qt::AlignHCenter);
        mWidgets << pLabel;

        col += colSpans[i];

        if (i < attributes.size() - 1)
        {
            QFrame *pVLine = WidgetManager::createLine(QFrame::VLine);
            ui->gridTable->addWidget(pVLine, 0, col++, -1, 1, Qt::AlignHCenter);
            mWidgets << pVLine;
        }
    }
}

void RaidProfitTable::initializeTableRow(const QList<int> &colSpans)
{
    int gateSize = mCosts.size();
    int row = 1;
    QStringList gateNameOfKayangel = {"천공의 문 넬라시아", "영원한 빛의 요람"};

    for (int i = 0; i < gateSize; i++)
    {
        int col = 0;

        QFrame *pHLine = WidgetManager::createLine(QFrame::HLine);
        ui->gridTable->addWidget(pHLine, row++, 0, 1, -1);
        mWidgets << pHLine;

        // 관문
        QLabel *pLabelGate = nullptr;

        if (mContent.contains("카양겔"))
            pLabelGate = WidgetManager::createLabel(gateNameOfKayangel[i], 10, "", 100);
        else
            pLabelGate = WidgetManager::createLabel(QString("%1관문").arg(i + 1), 10, "", 100);

        ui->gridTable->addWidget(pLabelGate, row, col, Qt::AlignHCenter);
        mWidgets << pLabelGate;

        col += colSpans[0] + 1 + colSpans[1] + 1;

        // 더보기 비용
        QLabel *pLabelCost = WidgetManager::createLabel(QString("%L1").arg(mCosts[i]), 10, "", 100);
        ui->gridTable->addWidget(pLabelCost, row, col, Qt::AlignHCenter);
        mWidgets << pLabelCost;

        col += colSpans[2] + 1;

        // 골드 가치 합계
        QLabel *pLabelTotalGold = WidgetManager::createLabel("-", 10, "", 100);
        ui->gridTable->addWidget(pLabelTotalGold, row, col, Qt::AlignHCenter);
        mGoldLabels << pLabelTotalGold;
        mWidgets << pLabelTotalGold;

        col += colSpans[3] + 1;

        // 더보기 손익
        QLabel *pLabelProfit = WidgetManager::createLabel("-", 10, "", 100);
        ui->gridTable->addWidget(pLabelProfit, row++, col, Qt::AlignHCenter);
        mProfitLabels << pLabelProfit;
        mWidgets << pLabelProfit;
    }
}

void RaidProfitTable::initializeTableReward()
{
    int row = 2;

    for (const RewardItems &rewardItems : mRewardItems)
    {
        int col = 2;

        for (const QPair<QString, int> &rewardItem : rewardItems)
        {
            QVBoxLayout *pLayoutItem = new QVBoxLayout();
            ui->gridTable->addLayout(pLayoutItem, row, col++, Qt::AlignHCenter);
            mLayouts << pLayoutItem;

            QLabel *pIconItem = WidgetManager::createIcon(ResourceManager::getInstance()->iconPath(rewardItem.first), nullptr);
            pLayoutItem->addWidget(pIconItem);
            mWidgets << pIconItem;

            QLabel *pLabelItemCount = WidgetManager::createLabel(QString("%L1").arg(rewardItem.second), 10, "", 100);
            pLayoutItem->addWidget(pLabelItemCount);
            mWidgets << pLabelItemCount;
        }

        row += 2;
    }
}
