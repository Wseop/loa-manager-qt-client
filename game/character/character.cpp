#include "character.h"

Character::Character()
{

}

Profile Character::profile() const
{
    return mProfile;
}

void Character::setProfile(const Profile &profile)
{
    mProfile = profile;
}

QHash<QString, Equipment> Character::equipments() const
{
    return mEquipments;
}

void Character::addEquipment(const Equipment &equipment)
{
    mEquipments[equipment.type] = equipment;
}

QList<Skill> Character::skills() const
{
    return mSkills;
}

void Character::addSkill(const Skill &skill)
{
    mSkills << skill;
}

QList<Gem> Character::gems() const
{
    return mGems;
}

void Character::addGem(const Gem &gem)
{
    mGems << gem;
}

QList<QPair<QString, int> > Character::engraves() const
{
    return mEngraves;
}

void Character::addEngrave(const QString &engraveName, int engraveLevel)
{
    mEngraves.append({engraveName, engraveLevel});
}

QList<QPair<QString, int> > Character::cards() const
{
    return mCards;
}

void Character::addCard(const QString &cardSet, int awaken)
{
    mCards.append({cardSet, awaken});
}

QList<Collectible> Character::collectibles() const
{
    return mCollectibles;
}

void Character::addCollectible(const Collectible &collectible)
{
    mCollectibles << collectible;
}
