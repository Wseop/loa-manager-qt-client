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
//    void initResultLayout();

    void searchAccessory(SearchOption& searchOption, AccessoryPart part);
    void clearResult();

private:
    Ui::SmartSearchAccessory *ui;

    // option
    QList<QStringList> m_optionItems;
    QList<class QComboBox*> m_optionSelectors;

    // search option code
    QHash<QString, int> m_abilityCodes;
    QHash<QString, CategoryCode> m_partCodes;

    // 검색 결과 & ui
    int m_responseCount;
    QList<QPair<Accessory*, int>> m_searchResults;
    QList<class QGridLayout*> m_resultLayouts;
    QList<int> m_currentLayoutRows;
    QList<QWidget*> m_itemWidgets;
    QList<QLayout*> m_itemLayouts;

    QList<QWidget*> m_widgets;
    QList<QLayout*> m_layouts;
};

#endif // SMART_SEARCH_ACCESSORY_H
