#ifndef REFORGE_QUOTATION_H
#define REFORGE_QUOTATION_H

#include "game_data/character/item/item.h"
#include <QWidget>
#include <QJsonObject>

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
    void addRefreshButton();
    void addReforgeItemWidgets();
    void refreshPrice();
    QJsonObject buildSearchOption(QString itemName);
    void sendRequest(class ReforgeItem* pReforgeItemWidget, QString itemName);

public:
    static ReforgeQuotation* getInstance();
    static void destroyInstance();

private:
    Ui::ReforgeQuotation *ui;

    static ReforgeQuotation* m_pInstance;

    QStringList m_categories;
    QList<QList<Item>> m_reforgeItems;
    class QPushButton* m_pBtnRefresh;
    QList<QList<ReforgeItem*>> m_reforgeItemWidgets;

    QList<class QLabel*> m_labels;
    QList<class QLayout*> m_layouts;
    QList<class QGroupBox*> m_groupBoxes;
};

#endif // REFORGE_QUOTATION_H
