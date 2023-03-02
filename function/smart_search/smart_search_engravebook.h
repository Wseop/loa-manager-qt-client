#ifndef SMART_SEARCH_ENGRAVEBOOK_H
#define SMART_SEARCH_ENGRAVEBOOK_H

#include "function/smart_search/smart_search_menu.h"

#include <QWidget>

namespace Ui {
class SmartSearchEngraveBook;
}

class SmartSearchEngraveBook : public QWidget, public SmartSearchMenu
{
    Q_OBJECT

public:
    SmartSearchEngraveBook(QLayout* pLayout);
    ~SmartSearchEngraveBook();

    void refresh() override;

private:
    void initializeUI();
    void updateUI();
    void clearUI();

    void updatePrice(bool bResetPageNo);

private:
    Ui::SmartSearchEngraveBook *ui;

    int m_searchPageNo;

    QStringList m_battleEngraveKeys;
    QStringList m_classEngraveKeys;
    QHash<QString, QPair<int, int>> m_engravePrices;
    QList<QWidget*> m_priceWidgets;

    QList<class QGridLayout*> m_layouts;
    QList<QWidget*> m_widgets;
};

#endif // SMART_SEARCH_ENGRAVEBOOK_H
