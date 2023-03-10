#include "armor.h"

Armor::Armor() :
    Item(ItemType::Armor)
{

}

ArmorPart Armor::armorPart() const
{
    return m_armorPart;
}

void Armor::setArmorPart(ArmorPart newArmorPart)
{
    m_armorPart = newArmorPart;
}

int Armor::quality() const
{
    return m_quality;
}

void Armor::setQuality(int newQuality)
{
    m_quality = newQuality;
}

int Armor::itemLevel() const
{
    return m_itemLevel;
}

void Armor::setItemLevel(int newItemLevel)
{
    m_itemLevel = newItemLevel;
}

int Armor::tier() const
{
    return m_tier;
}

void Armor::setTier(int newTier)
{
    m_tier = newTier;
}

int Armor::reforge() const
{
    return m_reforge;
}

void Armor::setReforge(int newReforge)
{
    m_reforge = newReforge;
}

ItemSet Armor::itemSet() const
{
    return m_itemSet;
}

void Armor::setItemSet(ItemSet newItemSet)
{
    m_itemSet = newItemSet;
}

int Armor::setLevel() const
{
    return m_setLevel;
}

void Armor::setSetLevel(int newSetLevel)
{
    m_setLevel = newSetLevel;
}

QList<Elixir> Armor::elixirs() const
{
    return m_elixirs;
}

void Armor::addElixir(const Elixir &elixir)
{
    m_elixirs << elixir;
}
