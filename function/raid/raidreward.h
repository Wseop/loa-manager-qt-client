#ifndef RAIDREWARD_H
#define RAIDREWARD_H

#include "function/raid/raidreward_profit.h"
#include <QWidget>
#include <QList>

namespace Ui {
class RaidReward;
}

class RaidReward : public QWidget
{
    Q_OBJECT

public:
    RaidReward(QString raid, QList<Reward> rewards, QMap<QString, QString> iconPaths);
    ~RaidReward();

    void updateItemPrice(QString item);

private:
    void addGateLabel(class QHBoxLayout* pLayout, QString gate);
    void addRewardLabel(QHBoxLayout* pLayout, QList<QPair<QString, int>> items);
    void addCostLabel(QHBoxLayout* pLayout, int cost);

    void updateProfit();

private:
    Ui::RaidReward *ui;

    QMap<QString, QString> m_iconPaths;
    QMap<QString, double> m_itemPrices;
    QList<QMap<QString, int>> m_itemCounts;
    QList<QMap<QString, class QLabel*>> m_itemPriceLabels;
    QList<int> m_costs;
    QList<QMap<QString, double>> m_itemTotalPrices;
    QList<class QLabel*> m_profitLabels;

    QList<QWidget*> m_widgets;
    QList<class QLayout*> m_layouts;
};

#endif // RAIDREWARD_H
