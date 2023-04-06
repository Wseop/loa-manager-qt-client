#ifndef ABILITYSTONE_H
#define ABILITYSTONE_H

#include "game/item/item.h"
#include "game/engrave/engrave.h"

class AbilityStone : public Item
{
public:
    AbilityStone();

    Engrave *getEngrave() const;

private:
    Engrave *mpEngrave;
};

#endif // ABILITYSTONE_H
