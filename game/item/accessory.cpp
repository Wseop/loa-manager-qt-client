#include "accessory.h"

Accessory::Accessory() :
    Item(ItemType::Accessory)
{

}

AccessoryPart Accessory::accessoryPart() const
{
    return m_accessoryPart;
}

void Accessory::setAccessoryPart(AccessoryPart newAccessoryPart)
{
    m_accessoryPart = newAccessoryPart;
}

int Accessory::quality() const
{
    return m_quality;
}

void Accessory::setQuality(int newQuality)
{
    m_quality = newQuality;
}

QHash<Ability, int> Accessory::abilities() const
{
    return m_abilities;
}

void Accessory::setAbility(Ability ability, int value)
{
    m_abilities[ability] = value;
}
