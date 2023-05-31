#ifndef ACCESSORY_INFO_H
#define ACCESSORY_INFO_H

#include "game/item/item_grade.h"
#include "game/item/equipment.h"

#include <QWidget>

namespace Ui {
class AccessoryInfo;
}

class AccessoryInfo : public QWidget
{
    Q_OBJECT

public:
    AccessoryInfo(const Equipment &accessory);
    ~AccessoryInfo();

private:
    void initializeLayout1(const QString &iconPath, const ItemGrade &itemGrade, int quality);
    void addAccessoryIcon(const QString &iconPath, ItemGrade itemGrade);
    void addQualityBar(int quality);

    void initializeLayout2(const ItemGrade &itemGrade, const QHash<QString, int> &abilities, const QHash<QString, int> &engraves);
    void addItemGradeInfo(const ItemGrade &itemGrade);
    void addAbilityInfo(const QHash<QString, int> &abilities);
    void addEngraveInfo(const QHash<QString, int> &engraves);

    class QHBoxLayout *createHLayout(class QVBoxLayout *pLayout);

private:
    Ui::AccessoryInfo *ui;
};

#endif // ACCESSORY_INFO_H
