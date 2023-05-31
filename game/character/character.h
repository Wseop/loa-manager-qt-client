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
    ~Character();

    Profile profile() const;
    void setProfile(const Profile &profile);

    QHash<QString, Equipment> equipments() const;
    Equipment equipment(const QString &type) const;
    QList<Equipment> armors() const;
    Equipment weapon() const;
    QList<Equipment> accessories() const;
    Equipment abilityStone() const;
    Equipment bracelet() const;
    void addEquipment(const Equipment &equipment);

    QList<Skill> skills() const;
    void addSkill(const Skill &skill);

    QList<Gem> gems() const;
    void addGem(const Gem &gem);

    QList<QPair<QString, int> > engraves() const;
    void addEngrave(const QPair<QString, int> &engrave);

    QList<QPair<QString, int> > cards() const;
    void addCard(const QPair<QString, int> &card);

    QList<Collectible> collectibles() const;
    void addCollectible(const Collectible &collectible);

    QList<Profile> siblings() const;
    void addSiblings(const Profile &sibling);

private:
    Profile mProfile;
    QHash<QString, Equipment> mEquipments;
    QList<Skill> mSkills;
    QList<Gem> mGems;
    QList<QPair<QString, int>> mEngraves;   // {engraveName, engraveLevel}
    QList<QPair<QString, int>> mCards;      // {cardSet, awaken}
    QList<Collectible> mCollectibles;

    QList<Profile> mSiblings;
};

#endif // CHARACTER_H
