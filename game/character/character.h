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
    void setProfile(const Profile &profile);

    QHash<QString, Equipment> equipments() const;
    void addEquipment(const Equipment &equipment);

    QList<Skill> skills() const;
    void addSkill(const Skill &skill);

    QList<Gem> gems() const;
    void addGem(const Gem &gem);

    QList<QPair<QString, int> > engraves() const;
    void addEngrave(const QString &engraveName, int engraveLevel);

    QList<QPair<QString, int> > cards() const;
    void addCard(const QString &cardSet, int awaken);

    QList<Collectible> collectibles() const;
    void addCollectible(const Collectible &collectible);

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
