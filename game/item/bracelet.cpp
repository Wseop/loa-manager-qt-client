#include "bracelet.h"

Bracelet::Bracelet() :
    Item(ItemType::Bracelet)
{

}

QList<QPair<QString, int>> Bracelet::effects() const
{
    return mEffects;
}

void Bracelet::addEffect(const QString &effect, int value)
{
    mEffects.append({effect, value});
}

QList<QPair<QString, int>> Bracelet::specialEffects() const
{
    return mSpecialEffects;
}

void Bracelet::addSpecialEffect(const QString &effect, int value)
{
    mSpecialEffects.append({effect, value});
}
