#include "abilitystone.h"

AbilityStone::AbilityStone() :
    Item(ItemType::어빌리티_스톤)
{

}

const QList<PairEngraveValue>& AbilityStone::getEngraves() const
{
    return m_engrave.getEngraves();
}

const QList<PairEngraveValue>& AbilityStone::getPenalties() const
{
    return m_engrave.getPenalties();
}

void AbilityStone::addEngrave(const QString& engrave, int value)
{
    m_engrave.addEngrave(engrave, value);
}

void AbilityStone::addPenalty(const QString& penalty, int value)
{
    m_engrave.addPenalty(penalty, value);
}

