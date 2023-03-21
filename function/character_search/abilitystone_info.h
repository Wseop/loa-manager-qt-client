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
    void initializeIcon(const QString &iconPath, ItemGrade itemGrade);
    void initializeItemNameInfo(const QString &itemName, ItemGrade itemGrade);
    void initializeEngraveInfo(const QStringList &engraves, const AbilityStone *pAbilityStone);
    void initializePenaltyInfo(const QString &penalty, int value);

    class QHBoxLayout *createHLayout();

private:
    Ui::AbilityStoneInfo *ui;

    QList<QWidget*> mWidgets;
    QList<QLayout*> mLayouts;
};

#endif // ABILITYSTONE_INFO_H
