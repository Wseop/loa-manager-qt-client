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
    void initializeIcon(const QString &iconPath, ItemGrade itemGrade);
    void initializeEffectInfo(const QList<QPair<QString, int>> &effects);
    void initializeSpecialEffectInfo(const QList<QPair<QString, int>> &specialEffects);

    class QHBoxLayout *createHLayout();

private:
    Ui::BraceletInfo *ui;

    QList<QWidget*> mWidgets;
    QList<QLayout*> mLayouts;
};

#endif // BRACELET_INFO_H
