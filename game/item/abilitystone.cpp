#include "abilitystone.h"

AbilityStone::AbilityStone() :
    Item(ItemType::AbilityStone),
    mpEngrave(new Engrave)
{

}

Engrave *AbilityStone::getEngrave() const
{
    return mpEngrave;
}
