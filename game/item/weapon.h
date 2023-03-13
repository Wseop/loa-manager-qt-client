#ifndef WEAPON_H
#define WEAPON_H

#include "game/item/item.h"
#include "game/item/item_set.h"

class Weapon : public Item
{
public:
    Weapon();

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

    bool ella() const;
    void setElla(bool newElla);

private:
    int mQuality;
    int mItemLevel;
    int mTier;
    int mReforge;
    ItemSet mItemSet;
    int mSetLevel;
    bool mbElla;
};

#endif // WEAPON_H
