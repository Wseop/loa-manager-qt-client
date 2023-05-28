#ifndef CHARACTER_H
#define CHARACTER_H

#include "game/character/profile.h"
#include "game/item/equipment.h"
#include "game/skill/skill.h"
#include "game/item/gem.h"
#include "game/character/collectible.h"

#include <QObject>
#include <QHash>

class Character : public QObject
{
    Q_OBJECT

public:
    Character();

    Profile profile() const;
    void setProfile(const Profile &newProfile);

private:
    Profile mProfile;
    QHash<QString, Equipment> mEquipments;
    QList<Skill> mSkills;
    QList<Gem> mGems;
    QList<QPair<QString, int>> mEngraves;   // {engraveName, engraveLevel}
    QList<QPair<QString, int>> mCards;      // {cardSet, awaken}
    QList<Collectible> mCollectibles;
};

#endif // CHARACTER_H
