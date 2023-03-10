#include "weapon.h"

Weapon::Weapon() :
    Item(ItemType::Weapon)
{

}

int Weapon::quality() const
{
    return m_quality;
}

void Weapon::setQuality(int newQuality)
{
    m_quality = newQuality;
}

int Weapon::itemLevel() const
{
    return m_itemLevel;
}

void Weapon::setItemLevel(int newItemLevel)
{
    m_itemLevel = newItemLevel;
}

int Weapon::iter() const
{
    return m_iter;
}

void Weapon::setIter(int newIter)
{
    m_iter = newIter;
}

int Weapon::reforge() const
{
    return m_reforge;
}

void Weapon::setReforge(int newReforge)
{
    m_reforge = newReforge;
}

ItemSet Weapon::itemSet() const
{
    return m_itemSet;
}

void Weapon::setItemSet(ItemSet newItemSet)
{
    m_itemSet = newItemSet;
}

int Weapon::setLevel() const
{
    return m_setLevel;
}

void Weapon::setSetLevel(int newSetLevel)
{
    m_setLevel = newSetLevel;
}

bool Weapon::ella() const
{
    return m_ella;
}

void Weapon::setElla(bool newElla)
{
    m_ella = newElla;
}
