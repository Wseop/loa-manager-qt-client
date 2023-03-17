#include "armor.h"

Armor::Armor() :
    Item(ItemType::Armor),
    mReforge(0),
    mItemSet(ItemSet::size),
    mSetLevel(0)
{

}

ArmorPart Armor::armorPart() const
{
    return mArmorPart;
}

void Armor::setArmorPart(ArmorPart newArmorPart)
{
    mArmorPart = newArmorPart;
}

int Armor::quality() const
{
    return mQuality;
}

void Armor::setQuality(int newQuality)
{
    mQuality = newQuality;
}

int Armor::itemLevel() const
{
    return mItemLevel;
}

void Armor::setItemLevel(int newItemLevel)
{
    mItemLevel = newItemLevel;
}

int Armor::tier() const
{
    return mTier;
}

void Armor::setTier(int newTier)
{
    mTier = newTier;
}

int Armor::reforge() const
{
    return mReforge;
}

void Armor::setReforge(int newReforge)
{
    mReforge = newReforge;
}

ItemSet Armor::itemSet() const
{
    return mItemSet;
}

void Armor::setItemSet(ItemSet newItemSet)
{
    mItemSet = newItemSet;
}

int Armor::setLevel() const
{
    return mSetLevel;
}

void Armor::setSetLevel(int newSetLevel)
{
    mSetLevel = newSetLevel;
}

QList<Elixir> Armor::elixirs() const
{
    return mElixirs;
}

void Armor::addElixir(const Elixir &elixir)
{
    mElixirs << elixir;
}

void Armor::addElixir(const QString &part, const QString &effect, int level)
{
    const QStringList parts = {"투구", "어깨", "상의", "하의", "장갑", "공용"};

    addElixir({static_cast<ArmorPart>(parts.indexOf(part)), effect, level});
}
