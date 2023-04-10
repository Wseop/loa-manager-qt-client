#ifndef CONTENT_REWARD_H
#define CONTENT_REWARD_H

#include "function/function_widget.h"
#include "api/loamanager/loamanager_api.h"

#include <QWidget>

namespace Ui {
class ContentReward;
}

class ContentReward : public QWidget, public FunctionWidget
{
    Q_OBJECT

private:
    ContentReward();
    ~ContentReward();

    void initializeContentData();
    void initializeSearchOption();
    void initializeContentSelector();
    void initializeTradableSelector();
    void initializeRewardTable();
    void initializeRewardAdder();

    void refreshRewardData();
    void refreshTradablePrice();

public:
    static ContentReward *getInstance();
    static void destroyInstance();

public:
    void start() override;

private:
    static ContentReward *m_pInstance;

private:
    Ui::ContentReward *ui;

    QStringList mContents;
    QHash<QString, QStringList> mContentLevels;
    QHash<QString, QStringList> mDropTable;
    QHash<QString, Reward> mRewardData;
    QHash<QString, int> mTradablePrice;

    int mTotalLevels;
    int mResponseCount;

    // 거래소, 경매장 검색
    QList<class SearchOption*> mSearchOptions;

    // selector
    class QComboBox *mpContentSelector;
    QHash<QString, class QCheckBox*> mTradableSelector;

    // reward table
    QList<class ContentRewardTable*> mRewardTables;
    ContentRewardTable *mpSelectedTable;

    // reward adder
    class ContentRewardAdder *mpRewardAdder;

    class QLabel *mpLabelInfo;

    QList<QWidget*> mWidgets;
    QList<QLayout*> mLayouts;
};

#endif // CONTENT_REWARD_H
