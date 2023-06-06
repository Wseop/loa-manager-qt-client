#ifndef RAID_PROFIT_WIDGET_H
#define RAID_PROFIT_WIDGET_H

#include "api/resources/reward.h"

#include <QWidget>

class QLabel;
class QGroupBox;
class QHBoxLayout;
class QVBoxLayout;

namespace Ui {
class RaidProfitWidget;
}

class RaidProfitWidget : public QWidget
{
    Q_OBJECT

public:
    RaidProfitWidget(const ContentReward &contentReward);
    ~RaidProfitWidget();

    void updateProfit(const QStringList &selectedItems);

private:
    void initializeContentLayout(const QString &content);
    QLabel *createContentLabel(const QString &content);

    void initializeRewardLayout(const QList<Reward>& rewards);
    QHBoxLayout *createLevelLayout(const Reward &reward);
    QLabel *createGateLabel(const QString &gate);
    QGroupBox *createCostGroup(const int cost);
    QGroupBox *createItemGroup(const QList<RewardItem> &items);
    QVBoxLayout *createItemLayout(const RewardItem &item);
    QGroupBox *createProfitGroup();
    QLabel *createProfitLabel();

    void addItemCount(const QList<RewardItem> &items);

private:
    Ui::RaidProfitWidget *ui;

    QList<QHash<QString, int>> mItemCountMap;
    QList<int> mCosts;
    QList<QLabel*> mProfitLabels;
};

#endif // RAID_PROFIT_WIDGET_H
