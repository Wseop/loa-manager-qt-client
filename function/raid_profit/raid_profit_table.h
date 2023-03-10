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

    QString m_content;
    QList<int> m_costs;
    QList<RewardItems> m_rewardItems;

    QList<class QLabel*> m_goldLabels;
    QList<QLabel*> m_profitLabels;

    QList<QWidget*> m_widgets;
    QList<QLayout*> m_layouts;
};

#endif // RAID_PROFIT_TABLE_H
