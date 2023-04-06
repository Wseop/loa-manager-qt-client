#include "accessory.h"

Accessory::Accessory() :
    Item(ItemType::Accessory),
    mpEngrave(new Engrave)
{

}

AccessoryPart Accessory::accessoryPart() const
{
    return mAccessoryPart;
}

void Accessory::setAccessoryPart(AccessoryPart newAccessoryPart)
{
    mAccessoryPart = newAccessoryPart;
}

int Accessory::quality() const
{
    return mQuality;
}

void Accessory::setQuality(int newQuality)
{
    mQuality = newQuality;
}

QHash<Ability, int> Accessory::abilities() const
{
    return mAbilities;
}

void Accessory::setAbility(Ability ability, int value)
{
    mAbilities[ability] = value;
}

Engrave *Accessory::getEngrave() const
{
    return mpEngrave;
}
