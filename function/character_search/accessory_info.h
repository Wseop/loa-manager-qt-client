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
    void initializeIcon(const QString &iconPath, ItemGrade itemGrade);
    void initializeQualityBar(int quality);
    void initializeItemGradeInfo(ItemGrade itemGrade);
    void initializeAbilityInfo(const QHash<Ability, int> &abilities);
    void initializeEngraveInfo(const QStringList &engraves, const Accessory *pAccessory);
    void initializePenaltyInfo(const QStringList &penalties, const Accessory *pAccessory);

    class QHBoxLayout *createHLayout();

private:
    Ui::AccessoryInfo *ui;

    QList<QWidget*> mWidgets;
    QList<QLayout*> mLayouts;
};

#endif // ACCESSORY_INFO_H
