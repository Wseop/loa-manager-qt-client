#include "bracelet.h"

Bracelet::Bracelet() :
    Item(ItemType::Bracelet)
{

}

QList<QPair<QString, int>> Bracelet::effects() const
{
    return m_effects;
}

void Bracelet::addEffect(const QString &effect, int value)
{
    m_effects.append({effect, value});
}

QList<QPair<QString, int>> Bracelet::specialEffects() const
{
    return m_specialEffects;
}

void Bracelet::addSpecialEffect(const QString &effect, int value)
{
    m_specialEffects.append({effect, value});
}
