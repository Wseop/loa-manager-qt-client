#ifndef ABILITYSTONE_H
#define ABILITYSTONE_H

#include "game/item/item.h"
#include "game/engrave/engrave.h"

class AbilityStone : public Item, public Engrave
{
public:
    AbilityStone();
};

#endif // ABILITYSTONE_H
