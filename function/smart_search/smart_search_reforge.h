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

    QStringList mCategories;
    QList<QList<Item>> mItems;

    QList<class QLabel*> mRecentPriceLabels;
    QList<QLabel*> mMinPriceLabels;
    QList<QLabel*> mEfficiencyLabels;

    QList<QLayout*> mLayouts;
    QList<QWidget*> mWidgets;
};

#endif // SMART_SEARCH_REFORGE_H
