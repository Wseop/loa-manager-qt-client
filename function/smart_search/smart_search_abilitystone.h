#ifndef SMART_SEARCH_ABILITYSTONE_H
#define SMART_SEARCH_ABILITYSTONE_H

#include "function/smart_search/smart_search_menu.h"

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
    void initMenu();
    void initResultLayout();
    void searchAbilityStone();
    void addSearchResult(const QStringList& engraves, const int& price);

private:
    Ui::SmartSearchAbilityStone *ui;

    const int ENGRAVE_OPTION_CODE = 3;

    QList<class EngraveSelector*> m_engraveSelectors;
    QList<class QPushButton*> m_engraveSelectButtons;
    int m_addCount;

    QList<QWidget*> m_widgets;
    QList<QLayout*> m_layouts;
};

#endif // SMART_SEARCH_ABILITYSTONE_H
