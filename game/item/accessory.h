#ifndef ACCESSORY_H
#define ACCESSORY_H

#include "game/item/item.h"
#include "game/engrave/engrave.h"
#include "game/character/ability.h"

#include <QHash>

enum class AccessoryPart
{
    Necklace, Earring, Ring
};

class Accessory : public Item, public Engrave
{
public:
    Accessory();

    AccessoryPart accessoryPart() const;
    void setAccessoryPart(AccessoryPart newAccessoryPart);

    int quality() const;
    void setQuality(int newQuality);

    QHash<Ability, int> abilities() const;
    void setAbility(Ability ability, int value);

private:
    AccessoryPart m_accessoryPart;
    int m_quality;
    QHash<Ability, int> m_abilities;
};

#endif // ACCESSORY_H
