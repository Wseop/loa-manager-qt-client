#include "bracelet.h"

Bracelet::Bracelet() :
    Item(ItemType::팔찌)
{

}

const QStringList& Bracelet::getEffects() const
{
    return m_effects;
}

const QStringList& Bracelet::getSpecialEffects() const
{
    return m_specialEffects;
}

void Bracelet::addEffect(const QString& effect)
{
    if (effect.startsWith("["))
        m_specialEffects << effect;
    else
        m_effects << effect;
}
