#ifndef SMART_SEARCH_REFORGE_H
#define SMART_SEARCH_REFORGE_H

#include "function/smart_search/smart_search_menu.h"
#include "game/item/item.h"

#include <QWidget>

namespace Ui {
class SmartSearchReforge;
}

class SmartSearchReforge : public QWidget, public SmartSearchMenu
{
    Q_OBJECT

public:
    SmartSearchReforge(QLayout *pLayout);
    ~SmartSearchReforge();

    void refresh() override;

private:
    void loadResource();
    void initializeUI();

private:
    Ui::SmartSearchReforge *ui;

    QStringList m_categories;
    QList<QList<Item>> m_items;

    QList<class QLabel*> m_recentPriceLabels;
    QList<QLabel*> m_minPriceLabels;
    QList<QLabel*> m_efficiencyLabels;

    QList<QLayout*> m_layouts;
    QList<QWidget*> m_widgets;
};

#endif // SMART_SEARCH_REFORGE_H
