#ifndef ACCESSORY_H
#define ACCESSORY_H

#include "game_data/character/item/item.h"
#include "game_data/character/engrave/engrave.h"
#include "game_data/character/profile/enum/ability.h"

class Accessory : public Item
{
public:
    Accessory(ItemType type);

    const int& getQuality() const;
    const QMap<Ability, int>& getAbilities() const;
    const QMap<QString, int>& getEngraves() const;
    const QMap<QString, int>& getPenalties() const;

    void setQuality(int quality);
    void addAbility(Ability ability, int value);
    void addEngrave(const QString& engrave, int value);
    void addPenalty(const QString& penalty, int value);

private:
    int m_quality;
    QMap<Ability, int> m_abilities;
    Engrave m_engrave;
};

#endif // ACCESSORY_H
