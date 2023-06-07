#ifndef TRIPOD_SEARCH_H
#define TRIPOD_SEARCH_H

#include "function/function_widget.h"
#include "api/lostark/item_manager.h"

#include <QWidget>

class QGroupBox;
class QComboBox;

class TripodSearchItem;

namespace Ui {
class TripodSearch;
}

class TripodSearch : public QWidget, public FunctionWidget
{
    Q_OBJECT

private:
    TripodSearch();
    ~TripodSearch();

    void initializeInputLayout();
    QGroupBox *createClassSelectorGroup();
    QComboBox *createClassSelector();
    QPushButton *createSearchButton();

    QList<QPair<QPair<QString, int>, QPair<QString, int>>> getTripods();
    void searchTripod(const QList<QPair<QPair<QString, int>, QPair<QString, int>>> &tripodCodes);
    void showSearchResult();

public:
    void refresh() override;

private:
    Ui::TripodSearch *ui;

    QString mSelectedClass;

    // search result
    int mSearchHandleCount;
    QList<AuctionItem> mSearchResults;
    QList<TripodSearchItem*> mTripodSearchItems;

    // ui
    QComboBox *pClassSelector;
    QPushButton *pSearchButton;

public:
    static TripodSearch *getInstance();
    static void destroyInstance();

private:
    static TripodSearch *pInstance;
};

#endif // TRIPOD_SEARCH_H
