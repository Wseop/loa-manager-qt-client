#ifndef SMART_SEARCH_ENGRAVEBOOK_H
#define SMART_SEARCH_ENGRAVEBOOK_H

#include "function/smart_search/smart_search_menu.h"
#include "api/lostark/response_market.h"

#include <QWidget>

namespace Ui {
class SmartSearchEngraveBook;
}

class SmartSearchEngraveBook : public QWidget, public SmartSearchMenu
{
    Q_OBJECT

public:
    SmartSearchEngraveBook(QLayout *pLayout);
    ~SmartSearchEngraveBook();

    void refresh() override;

private:
    void initializeUI();
    void updateUI();
    void clearUI();

    void searchEngraveBook(bool bResetPageNo);
    void parseSearchResult(class QNetworkReply *pReply);

private:
    Ui::SmartSearchEngraveBook *ui;

    int mSearchPageNo;

    QList<MarketItem> mSearchResults;
    QList<QWidget*> mPriceWidgets;

    QList<class QGridLayout*> mLayouts;
    QList<QWidget*> mWidgets;
};

#endif // SMART_SEARCH_ENGRAVEBOOK_H
