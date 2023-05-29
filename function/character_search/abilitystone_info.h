#ifndef ABILITYSTONE_INFO_H
#define ABILITYSTONE_INFO_H

#include "game/item/item_grade.h"
#include "game/item/equipment.h"

#include <QWidget>

namespace Ui {
class AbilityStoneInfo;
}

class AbilityStoneInfo : public QWidget
{
    Q_OBJECT

public:
    AbilityStoneInfo(const Equipment &abilityStone);
    ~AbilityStoneInfo();

private:
    void initializeLayout1(const QString &iconPath, const ItemGrade &itemGrade);
    void addAbilityStoneIcon(const QString &iconPath, ItemGrade itemGrade);

    void initializeLayout2(const QString &itemName, const ItemGrade &itemGrade, const QHash<QString, int> &engraves);
    void addItemNameInfo(const QString &itemName, ItemGrade itemGrade);
    void addEngraveInfo(const QHash<QString, int> &engraves);

    class QHBoxLayout *createHLayout(class QVBoxLayout *pLayout);

private:
    Ui::AbilityStoneInfo *ui;
};

#endif // ABILITYSTONE_INFO_H
