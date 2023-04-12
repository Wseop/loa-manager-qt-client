#ifndef ABILITYSTONE_INFO_H
#define ABILITYSTONE_INFO_H

#include "game/item/item_grade.h"

#include <QWidget>

namespace Ui {
class AbilityStoneInfo;
}

class AbilityStoneInfo : public QWidget
{
    Q_OBJECT

public:
    AbilityStoneInfo(const class AbilityStone *pAbilityStone);
    ~AbilityStoneInfo();

private:
    void initializeLayout1(const AbilityStone *pAbilityStone);
    void addAbilityStoneIcon(const QString &iconPath, ItemGrade itemGrade);

    void initializeLayout2(const AbilityStone *pAbilityStone);
    void addItemNameInfo(const QString &itemName, ItemGrade itemGrade);
    void addEngraveInfo(const QStringList &engraves, const QStringList &penalties, const AbilityStone *pAbilityStone);

    class QHBoxLayout *createHLayout(class QVBoxLayout *pLayout);

private:
    Ui::AbilityStoneInfo *ui;
};

#endif // ABILITYSTONE_INFO_H
