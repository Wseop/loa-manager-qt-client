#ifndef ACCESSORY_H
#define ACCESSORY_H

#include "game/item/item.h"
#include "game/engrave/engrave.h"
#include "game/character/ability.h"

#include <QHash>

enum class AccessoryPart
{
    Necklace = 0, Earring = 1, Ring = 3
};

class Accessory : public Item
{
public:
    Accessory();

    AccessoryPart accessoryPart() const;
    void setAccessoryPart(AccessoryPart newAccessoryPart);

    int quality() const;
    void setQuality(int newQuality);

    QHash<Ability, int> abilities() const;
    void setAbility(Ability ability, int value);

    Engrave *getEngrave() const;

private:
    AccessoryPart mAccessoryPart;
    int mQuality;
    QHash<Ability, int> mAbilities;
    Engrave *mpEngrave;
};

#endif // ACCESSORY_H
