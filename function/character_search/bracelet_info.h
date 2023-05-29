#ifndef BRACELET_INFO_H
#define BRACELET_INFO_H

#include "game/item/item_grade.h"
#include "game/item/equipment.h"

#include <QWidget>

namespace Ui {
class BraceletInfo;
}

class BraceletInfo : public QWidget
{
    Q_OBJECT

public:
    BraceletInfo(const Equipment &bracelet);
    ~BraceletInfo();

private:
    void initializeLayout1(const QString &iconPath, const ItemGrade &itemGrade);
    void addBraceletIcon(const QString &iconPath, ItemGrade itemGrade);

    void initializeLayout2(const QStringList &effects);
    void addEffectInfo(const QStringList &effects);
    void addSpecialEffectInfo(const QStringList &specialEffects);

    class QHBoxLayout *createHLayout(class QVBoxLayout *pLayout);

private:
    Ui::BraceletInfo *ui;
};

#endif // BRACELET_INFO_H
