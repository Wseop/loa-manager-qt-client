#ifndef RAID_PROFIT_H
#define RAID_PROFIT_H

#include "function/function_widget.h"
#include "api/lostark/search_option.h"

#include <QWidget>

using RewardItems = QList<QPair<QString, int>>;

namespace Ui {
class RaidProfit;
}

class RaidProfit : public QWidget, public FunctionWidget
{
    Q_OBJECT

private:
    RaidProfit();
    ~RaidProfit();

    void initializeRaidReward();
    void initializeSearchOption();
    void initializeContentSelect();
    void initializeItemSelect();
    void initializeProfitTable();

    void refreshItemPrice();

public:
    static RaidProfit *getInstance();
    static void destroyInstance();

public:
    void start() override;

private:
    Ui::RaidProfit *ui;

    // Raid Reward
    QStringList mContents;
    QHash<QString, double> mItemPrice;
    QList<QList<int>> mCosts;
    QList<QList<RewardItems>> mRewardItems;

    // 아이템 가격 검색
    QList<SearchOption*> mSearchOptions;
    int mResponseCount;

    class QComboBox *mpContentSelector;
    QList<class QCheckBox*> mItemSelectors;

    // 레이드 더보기 보상/손익 테이블
    QList<class RaidProfitTable*> mProfitTables;
    RaidProfitTable *mpCurrentTable;

private:
    static RaidProfit *mpInstance;
};

#endif // RAID_PROFIT_H
