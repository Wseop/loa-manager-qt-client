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
    void initializeLayout1(const Item *pEquip, ItemType itemType);
    void addEquipIcon(const QString &iconPath, ItemGrade itemGrade);
    void addQualityBar(int quality);

    void initializeLayout2(const Item *pEquip, ItemType itemType);
    void addItemSourceInfo(ItemSet itemSet, const QString &itemName, ItemGrade itemGrade);
    void addReforgeLevelInfo(int reforge, int itemLevel);
    void addItemSetInfo(ItemSet itemSet, int setLevel);
    void addElixirInfo(const QList<Elixir> &elixirs, ArmorPart part);
    void addEllaInfo(bool bElla);

private:
    Ui::EquipInfo *ui;

    QList<QWidget*> mWidgets;
    QList<QLayout*> mLayouts;
};

#endif // EQUIP_INFO_H
