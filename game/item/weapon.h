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

    int iter() const;
    void setIter(int newIter);

    int reforge() const;
    void setReforge(int newReforge);

    ItemSet itemSet() const;
    void setItemSet(ItemSet newItemSet);

    int setLevel() const;
    void setSetLevel(int newSetLevel);

    bool ella() const;
    void setElla(bool newElla);

private:
    int m_quality;
    int m_itemLevel;
    int m_iter;
    int m_reforge;
    ItemSet m_itemSet;
    int m_setLevel;
    bool m_ella;
};

#endif // WEAPON_H
