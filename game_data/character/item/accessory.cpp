#include "accessory.h"

Accessory::Accessory(ItemType type) :
    Item(type),
    m_quality(0)
{

}

const int& Accessory::getQuality() const
{
    return m_quality;
}

const QMap<Ability, int>& Accessory::getAbilities() const
{
    return m_abilities;
}

const QMap<QString, int>& Accessory::getEngraves() const
{
    return m_engrave.getEngraves();
}

const QMap<QString, int>& Accessory::getPenalties() const
{
    return m_engrave.getPenalties();
}

void Accessory::setQuality(int quality)
{
    m_quality = quality;
}

void Accessory::addAbility(Ability ability, int value)
{
    m_abilities[ability] = value;
}

void Accessory::addEngrave(const QString& engrave, int value)
{
    m_engrave.addEngrave(engrave, value);
}

void Accessory::addPenalty(const QString& penalty, int value)
{
    m_engrave.addPenalty(penalty, value);
}

