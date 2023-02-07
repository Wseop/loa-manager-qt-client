#ifndef REFORGE_ITEM_H
#define REFORGE_ITEM_H

#include "game_data/character/item/enum/item_grade.h"
#include <QWidget>

namespace Ui {
class ReforgeItem;
}

class ReforgeItem : public QWidget
{
    Q_OBJECT

public:
    ReforgeItem(QString name, QString iconPath, ItemGrade grade);
    ~ReforgeItem();

    void setPrice(int price);

private:
    void addIcon(QString iconPath, ItemGrade grade);
    void addLabels(QString name, ItemGrade grade);

private:
    Ui::ReforgeItem *ui;

    class QLabel* m_pIcon;
    QLabel* m_pLabelName;
    QLabel* m_pLabelPrice;
};

#endif // REFORGE_ITEM_H
