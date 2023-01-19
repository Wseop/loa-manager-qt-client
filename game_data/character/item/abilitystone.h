#ifndef ABILITYSTONE_H
#define ABILITYSTONE_H

#include "game_data/character/item/item.h"
#include "game_data/character/engrave/engrave.h"

class AbilityStone : public Item
{
public:
    AbilityStone();

    const QList<PairEngraveValue>& getEngraves() const;
    const QList<PairEngraveValue>& getPenalties() const;

    void addEngrave(const QString& engrave, int value);
    void addPenalty(const QString& penalty, int value);

private:
    Engrave m_engrave;
};

#endif // ABILITYSTONE_H
