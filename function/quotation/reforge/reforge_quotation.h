#ifndef REFORGE_QUOTATION_H
#define REFORGE_QUOTATION_H

#include "game_data/character/item/item.h"

#include <QWidget>

namespace Ui {
class ReforgeQuotation;
}

class ReforgeQuotation : public QWidget
{
    Q_OBJECT

private:
    ReforgeQuotation();
    ~ReforgeQuotation();

    void loadReforgeData();

    void initRefreshButton();
    void initReforgeItemWidget();
    void initEfficiencyWidget();

    void refreshPrice();
    void refreshEfficiency(class QLabel* pLabelEfficiency, QString itemName, int price);
    void sendApiRequest(class ReforgeItem* pReforgeItemWidget, QLabel* pLabelEfficiency, QString itemName);

public:
    static ReforgeQuotation* getInstance();
    static void destroyInstance();

private:
    Ui::ReforgeQuotation *ui;

    static ReforgeQuotation* m_pInstance;

    QStringList m_categories;
    QList<QList<Item>> m_reforgeItems;

    QList<QList<ReforgeItem*>> m_reforgeItemWidgets;
    QList<QList<QLabel*>> m_efficiencyLabels;

    QList<QWidget*> m_widgets;
    QList<QLayout*> m_layouts;
};

#endif // REFORGE_QUOTATION_H
