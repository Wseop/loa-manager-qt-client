#ifndef RAID_PROFIT_TABLE_H
#define RAID_PROFIT_TABLE_H

#include "function/raid_profit/raid_profit.h"

#include <QWidget>

namespace Ui {
class RaidProfitTable;
}

class RaidProfitTable : public QWidget
{
    Q_OBJECT

public:
    RaidProfitTable(const QString &content, const QList<int> &costs, const QList<RewardItems> &rewardItems);
    ~RaidProfitTable();

    void refreshItemPrice(const QHash<QString, double> &itemPrice, const QList<class QCheckBox*> &itemSelectors);

private:
    void initializeTable();
    void initializeTableColumn(const QList<int> &colSpans);
    void initializeTableRow(const QList<int> &colSpans);
    void initializeTableReward();

private:
    Ui::RaidProfitTable *ui;

    QString mContent;
    QList<int> mCosts;
    QList<RewardItems> mRewardItems;

    QList<class QLabel*> mGoldLabels;
    QList<QLabel*> mProfitLabels;
};

#endif // RAID_PROFIT_TABLE_H
