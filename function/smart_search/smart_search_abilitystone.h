#ifndef SMART_SEARCH_ABILITYSTONE_H
#define SMART_SEARCH_ABILITYSTONE_H

#include "function/smart_search/smart_search_menu.h"
#include "game/item/abilitystone.h"
#include "api/lostark/response_auction.h"

#include <QWidget>

namespace Ui {
class SmartSearchAbilityStone;
}

class SmartSearchAbilityStone : public QWidget, public SmartSearchMenu
{
    Q_OBJECT

public:
    SmartSearchAbilityStone(QLayout *pLayout);
    ~SmartSearchAbilityStone();

    void refresh() override;

private:
    void initializeValidEngraveList();
    void initializeFilter();
    void initializeResultUI();

    void searchAbilityStone();
    void parseSearchResult(class QNetworkReply *pReply);
    void showSearchResult();

    void clearResult();

    void applyFilter();

private:
    Ui::SmartSearchAbilityStone *ui;

    const int MAX_FILTER = 5;

    bool mIsWorking;

    QStringList mValidEngraves;
    QList<class QComboBox*> mEngraveSelectors;

    QList<QPair<AbilityStone, AuctionInfo>> mSearchResults;
    int mTotalSearchCount;
    int mCurrentSearchCount;

    QList<QList<QWidget*>> mResultWidgets;
    QHash<QString, QList<int>> mResultIndexMap;

    QStringList mEngraveFilters;
};

#endif // SMART_SEARCH_ABILITYSTONE_H
