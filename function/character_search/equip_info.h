#ifndef EQUIP_INFO_H
#define EQUIP_INFO_H

#include "game/item/item_grade.h"
#include "game/item/equipment.h"

#include <QWidget>

namespace Ui {
class EquipInfo;
}

class EquipInfo : public QWidget
{
    Q_OBJECT

public:
    EquipInfo(const Equipment &equipment);
    ~EquipInfo();

private:
    void initializeLayout1(const QString &iconPath, const ItemGrade &itemGrade, int quality);
    void addEquipIcon(const QString &iconPath, ItemGrade itemGrade);
    void addQualityBar(int quality);

    void initializeLayout2(const Equipment &equipment);
    void addItemSourceInfo(const QString &setName, const QString &itemName, ItemGrade itemGrade);
    void addReforgeLevelInfo(const QString &itemName, int itemLevel);
    void addItemSetInfo(const QPair<QString, int> &itemSet);
    void addElixirInfo(const QHash<QString, int> &elixirs);
    void addEllaInfo(bool bElla);

private:
    Ui::EquipInfo *ui;
};

#endif // EQUIP_INFO_H
