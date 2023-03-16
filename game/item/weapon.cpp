#include "weapon.h"

Weapon::Weapon() :
    Item(ItemType::Weapon),
    mReforge(0),
    mItemSet(ItemSet::size),
    mSetLevel(0),
    mbElla(false)
{

}

int Weapon::quality() const
{
    return mQuality;
}

void Weapon::setQuality(int newQuality)
{
    mQuality = newQuality;
}

int Weapon::itemLevel() const
{
    return mItemLevel;
}

void Weapon::setItemLevel(int newItemLevel)
{
    mItemLevel = newItemLevel;
}

int Weapon::tier() const
{
    return mTier;
}

void Weapon::setTier(int newTier)
{
    mTier = newTier;
}

int Weapon::reforge() const
{
    return mReforge;
}

void Weapon::setReforge(int newReforge)
{
    mReforge = newReforge;
}

ItemSet Weapon::itemSet() const
{
    return mItemSet;
}

void Weapon::setItemSet(ItemSet newItemSet)
{
    mItemSet = newItemSet;
}

int Weapon::setLevel() const
{
    return mSetLevel;
}

void Weapon::setSetLevel(int newSetLevel)
{
    mSetLevel = newSetLevel;
}

bool Weapon::ella() const
{
    return mbElla;
}

void Weapon::setElla(bool newElla)
{
    mbElla = newElla;
}
