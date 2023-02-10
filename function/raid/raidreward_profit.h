#ifndef RAIDREWARD_PROFIT_H
#define RAIDREWARD_PROFIT_H

#include <QWidget>
#include <QJsonObject>

namespace Ui {
class RaidRewardProfit;
}

struct Reward
{
    QString gate;
    int cost;
    QList<QPair<QString, int>> items;
};

class RaidRewardProfit : public QWidget
{
    Q_OBJECT

private:
    RaidRewardProfit();
    ~RaidRewardProfit();

    void setAlignments();

    void loadRewardData();
    void initIconPath();

    void addRefresh();
    void addRaidSelector();
    void addInfo();
    void addRaidRewardWidgets();

    void updatePrice();
    QJsonObject buildSearchOption(QString itemName);

public:
    static RaidRewardProfit* getInstance();
    static void destroyInstance();

    double getItemPrice(QString item);

private:
    Ui::RaidRewardProfit *ui;

    static RaidRewardProfit* m_pInstance;

    QStringList m_raids;
    // index1 : raid, index2 : gate
    QList<QList<Reward>> m_rewards;
    QMap<QString, QString> m_iconPaths;
    QMap<QString, double> m_itemPrices;

    QList<class RaidReward*> m_raidRewardWidgets;
    QList<QWidget*> m_widgets;
};

#endif // RAIDREWARD_PROFIT_H
