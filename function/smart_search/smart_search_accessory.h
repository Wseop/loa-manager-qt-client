#ifndef SMART_SEARCH_ACCESSORY_H
#define SMART_SEARCH_ACCESSORY_H

#include "function/smart_search/smart_search_menu.h"
#include "api/search_option.h"
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
    SmartSearchAccessory(QLayout* pLayout);
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
    void parseSearchResult(class QNetworkReply* pReply);
    void addSearchResult();
    void clearResult();

    // 검색 결과 widget 생성
    class QFrame* createHLine();
    class QLabel* createIcon(const QString& iconPath, const ItemGrade& itemGrade);
    QLabel* createLabelItemName(const QString& itemName, const ItemGrade& itemGrade);
    class QProgressBar* createQualityBar(const int& quality);
    class QVBoxLayout* createAbilityLayout(const QHash<Ability, int>& abilities);
    QVBoxLayout* createEngraveLayout(const Accessory* pAccessory);
    QLabel* createLabelPrice(const Price& price);

private:
    Ui::SmartSearchAccessory *ui;

    // option
    QList<QStringList> m_optionItems;
    QList<class QComboBox*> m_optionSelectors;

    // search option code
    QHash<QString, int> m_abilityCodes;
    QHash<QString, CategoryCode> m_partCodes;

    // 검색 상태 & 검색 결과
    int m_searchPage;
    int m_addCount;
    int m_responseCount;
    QList<class SearchOption*> m_searchOptions;
    QList<QPair<Accessory*, Price>> m_searchResults;

    // 검색 결과 ui
    QList<int> m_currentLayoutRows;
    class QPushButton* m_pButtonSearchMore;
    QList<QWidget*> m_itemWidgets;
    QList<QLayout*> m_itemLayouts;

    // ui
    QPushButton* m_pSearchButton;
    QList<class QGridLayout*> m_resultLayouts;
    QList<class QLabel*> m_engraveLabels;
    QList<QWidget*> m_widgets;
    QList<QLayout*> m_layouts;
};

#endif // SMART_SEARCH_ACCESSORY_H
