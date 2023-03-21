#ifndef BRACELET_INFO_H
#define BRACELET_INFO_H

#include "game/item/item_grade.h"

#include <QWidget>

namespace Ui {
class BraceletInfo;
}

class BraceletInfo : public QWidget
{
    Q_OBJECT

public:
    BraceletInfo(const class Bracelet *pBracelet);
    ~BraceletInfo();

private:
    void initializeLayout1(const Bracelet *pBracelet);
    void addBraceletIcon(const QString &iconPath, ItemGrade itemGrade);

    void initializeLayout2(const Bracelet *pBracelet);
    void addEffectInfo(const QList<QPair<QString, int>> &effects);
    void addSpecialEffectInfo(const QList<QPair<QString, int>> &specialEffects);

    class QHBoxLayout *createHLayout(class QVBoxLayout *pLayout);

private:
    Ui::BraceletInfo *ui;

    QList<QWidget*> mWidgets;
    QList<QLayout*> mLayouts;
};

#endif // BRACELET_INFO_H
