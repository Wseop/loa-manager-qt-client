#ifndef ACCESSORY_INFO_H
#define ACCESSORY_INFO_H

#include "game/item/item_grade.h"
#include "game/character/ability.h"

#include <QWidget>

namespace Ui {
class AccessoryInfo;
}

class AccessoryInfo : public QWidget
{
    Q_OBJECT

public:
    AccessoryInfo(const class Accessory *pAccessory);
    ~AccessoryInfo();

private:
    void initializeLayout1(const Accessory *pAccessory);
    void addAccessoryIcon(const QString &iconPath, ItemGrade itemGrade);
    void addQualityBar(int quality);

    void initializeLayout2(const Accessory *pAccessory);
    void addItemGradeInfo(ItemGrade itemGrade);
    void addAbilityInfo(const QHash<Ability, int> &abilities);
    void addEngraveInfo(const QStringList &engraves, const QStringList &penalties, const Accessory *pAccessory);

    class QHBoxLayout *createHLayout(class QVBoxLayout *pLayout);

private:
    Ui::AccessoryInfo *ui;
};

#endif // ACCESSORY_INFO_H
