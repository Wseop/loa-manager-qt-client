#ifndef SMART_SEARCH_ABILITYSTONE_H
#define SMART_SEARCH_ABILITYSTONE_H

#include "function/smart_search/smart_search_menu.h"
#include "game/item/abilitystone.h"

#include <QWidget>

namespace Ui {
class SmartSearchAbilityStone;
}

class SmartSearchAbilityStone : public QWidget, public SmartSearchMenu
{
    Q_OBJECT

public:
    SmartSearchAbilityStone(QLayout* pLayout);
    ~SmartSearchAbilityStone();

    void refresh() override;

private:
    void initializeEngraveSelector();
    void initializeResultUI();

    void searchAbilityStone();
    void clearResult();

private:
    Ui::SmartSearchAbilityStone *ui;

    QList<class QComboBox*> m_engraveSelectors;
    QComboBox* m_pPenaltySelector;
    QList<QPair<AbilityStone, Price>> m_searchResults;
    int m_searchCount;

    QList<QWidget*> m_resultWidgets;
    QList<QWidget*> m_widgets;
    QList<QLayout*> m_layouts;
};

#endif // SMART_SEARCH_ABILITYSTONE_H
