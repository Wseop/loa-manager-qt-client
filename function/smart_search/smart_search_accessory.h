#ifndef SMART_SEARCH_ACCESSORY_H
#define SMART_SEARCH_ACCESSORY_H

#include "function/smart_search/smart_search_menu.h"
#include "api/search_option.h"
#include "game_data/character/item/accessory.h"

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
    void initOption();
    void initOptionSelector();
    void initSearchOptionCode();
    void initSearchButton();
    void initResultLayout();

    void search(SearchOption& searchOption, ItemType itemType);
    void addItemToLayout(const Accessory& acc, const int& price);
    void clearResult();

private:
    Ui::SmartSearchAccessory *ui;

    const int ABILITY_OPTION_CODE = 2;
    const int ENGRAVE_OPTION_CODE = 3;
    const QList<int> LABEL_WIDTHS = {50, 125, 50, 75, 125, 75};

    // option
    QList<QStringList> m_optionItems;
    QList<class EngraveSelector*> m_engraveSelectors;
    QList<QWidget*> m_optionSelectors;

    // search option code
    QHash<QString, int> m_abilityCodes;
    QHash<QString, CategoryCode> m_partCodes;

    QList<class QGridLayout*> m_resultLayouts;
    QList<int> m_currentLayoutRows;
    QList<QWidget*> m_itemWidgets;
    QList<QLayout*> m_itemLayouts;

    QList<QWidget*> m_widgets;
    QList<class QSpacerItem*> m_spacers;
};

#endif // SMART_SEARCH_ACCESSORY_H
