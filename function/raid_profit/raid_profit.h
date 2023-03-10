#ifndef RAID_PROFIT_H
#define RAID_PROFIT_H

#include "function/function_widget.h"
#include "api/search_option.h"

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
    QStringList m_contents;
    QHash<QString, double> m_itemPrice;
    QList<QList<int>> m_costs;
    QList<QList<RewardItems>> m_rewardItems;

    // 아이템 가격 검색
    QList<SearchOption*> m_searchOptions;
    int m_responseCount;

    class QComboBox *m_pContentSelector;
    QList<class QCheckBox*> m_itemSelectors;

    // 레이드 더보기 보상/손익 테이블
    QList<class RaidProfitTable*> m_profitTables;
    RaidProfitTable *m_pCurrentTable;

    QList<QWidget*> m_widgets;
    QList<QLayout*> m_layouts;

private:
    static RaidProfit *m_pInstance;
};

#endif // RAID_PROFIT_H
