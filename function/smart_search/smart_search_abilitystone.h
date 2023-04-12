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
    void initializeEngraveSelector();
    void initializeResultUI();

    void searchAbilityStone();
    void parseSearchResult(class QNetworkReply *pReply);
    void clearResult();

private:
    Ui::SmartSearchAbilityStone *ui;

    QList<class QComboBox*> mEngraveSelectors;
    QComboBox *mpPenaltySelector;

    QList<QPair<AbilityStone, AuctionInfo>> mSearchResults;
    int mTotalSearchCount;
    int mCurrentSearchCount;

    QList<QWidget*> mResultWidgets;
};

#endif // SMART_SEARCH_ABILITYSTONE_H
