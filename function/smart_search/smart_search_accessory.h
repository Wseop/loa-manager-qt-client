#ifndef SMART_SEARCH_ACCESSORY_H
#define SMART_SEARCH_ACCESSORY_H

#include "function/smart_search/smart_search_menu.h"
#include "api/lostark/search_option.h"
#include "api/lostark/response_auction.h"
#include "game/item/accessory.h"

#include <QWidget>

enum OptionIndex
{
    Grade, Part, Ability1, Ability2, Quality, Engrave1, Engrave2, Penalty
};

namespace Ui {
class SmartSearchAccessory;
}

class SmartSearchAccessory : public QWidget, public SmartSearchMenu
{
    Q_OBJECT

public:
    SmartSearchAccessory(QLayout *pLayout);
    ~SmartSearchAccessory();

    void refresh() override;

private:
    void initializeOption();
    void initializeOptionSelector();
    void initializeOptionCode();
    void initializeSearchButton();
    void initializeResultUI();
    void initializeSearchMore();

    void setSearchOption();
    void searchAccessory();
    void parseSearchResult(class QNetworkReply *pReply);
    void addSearchResult();
    void clearResult();

    // 검색 결과 widget 생성
    class QFrame* createHLine();
    class QLabel* createIcon(const QString &iconPath, const ItemGrade &itemGrade);
    QLabel *createLabelItemName(const QString &itemName, const ItemGrade &itemGrade);
    class QProgressBar* createQualityBar(const int &quality);
    class QVBoxLayout* createAbilityLayout(const QHash<Ability, int> &abilities);
    QVBoxLayout *createEngraveLayout(const Accessory *pAccessory);
    QLabel *createLabelPrice(const AuctionInfo &auctionInfo);

private:
    Ui::SmartSearchAccessory *ui;

    // option
    QList<QStringList> mOptionItems;
    QList<class QComboBox*> mOptionSelectors;

    // search option code
    QHash<QString, int> mAbilityCodes;
    QHash<QString, CategoryCode> mPartCodes;

    // 검색 상태 & 검색 결과
    int mSearchPage;
    int mAddCount;
    int mResponseCount;
    QList<class SearchOption*> mSearchOptions;
    QList<QPair<Accessory*, AuctionInfo>> mSearchResults;

    // 검색 결과 ui
    QList<int> mCurrentLayoutRows;
    class QPushButton* mpButtonSearchMore;
    QList<QWidget*> mItemWidgets;
    QList<QLayout*> mItemLayouts;

    // ui
    QPushButton *mpSearchButton;
    QList<class QGridLayout*> mResultLayouts;
    QList<class QLabel*> mEngraveLabels;
    QList<QWidget*> mWidgets;
    QList<QLayout*> mLayouts;
};

#endif // SMART_SEARCH_ACCESSORY_H
