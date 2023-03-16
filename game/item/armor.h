#ifndef ARMOR_H
#define ARMOR_H

#include "game/item/item.h"
#include "game/item/item_set.h"

#include <QList>

enum class ArmorPart
{
    Head, Shoulder, Top, Bottom, Hand, size
};

struct Elixir
{
    ArmorPart part;
    QString effect;
    int level;
};

class Armor : public Item
{
public:
    Armor();

    ArmorPart armorPart() const;
    void setArmorPart(ArmorPart newArmorPart);

    int quality() const;
    void setQuality(int newQuality);

    int itemLevel() const;
    void setItemLevel(int newItemLevel);

    int tier() const;
    void setTier(int newTier);

    int reforge() const;
    void setReforge(int newReforge);

    ItemSet itemSet() const;
    void setItemSet(ItemSet newItemSet);

    int setLevel() const;
    void setSetLevel(int newSetLevel);

    QList<Elixir> elixirs() const;
    void addElixir(const Elixir &elixir);

private:
    ArmorPart mArmorPart;
    int mQuality;
    int mItemLevel;
    int mTier;
    int mReforge;
    ItemSet mItemSet;
    int mSetLevel;
    QList<Elixir> mElixirs;
};

#endif // ARMOR_H
