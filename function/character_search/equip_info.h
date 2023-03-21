#ifndef EQUIP_INFO_H
#define EQUIP_INFO_H

#include "game/item/item_grade.h"
#include "game/item/item_set.h"
#include "game/item/armor.h"

#include <QWidget>

namespace Ui {
class EquipInfo;
}

class EquipInfo : public QWidget
{
    Q_OBJECT

public:
    EquipInfo(const class Item *pEquip);
    ~EquipInfo();

private:
    void initializeIcon(const QString &iconPath, ItemGrade itemGrade);
    void initializeQualityBar(int quality);
    void initializeReforgeLevel(int reforge, int itemLevel, ItemGrade itemGrade);
    void initializeItemSet(ItemSet itemSet, int setLevel);
    void initializeElixir(const QList<Elixir> &elixirs, ArmorPart part);
    void initializeElla(bool bElla);

private:
    Ui::EquipInfo *ui;

    QList<QWidget*> mWidgets;
    QList<QLayout*> mLayouts;
};

#endif // EQUIP_INFO_H
