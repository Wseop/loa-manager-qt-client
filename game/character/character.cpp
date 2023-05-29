#include "character.h"

Character::Character()
{

}

Character::~Character()
{
    for (const Skill &skill: mSkills) {
        if (skill.pRune != nullptr) {
            delete skill.pRune;
        }
    }
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

Equipment Character::equipment(const QString &type) const
{
    if (mEquipments.contains(type)) {
        return mEquipments[type];
    } else {
        return Equipment{};
    }
}

QList<Equipment> Character::armors() const
{
    QList<Equipment> armors;

    const QStringList types = {"투구", "상의", "하의", "장갑", "어깨"};

    for (const QString &type : types) {
        armors << equipment(type);
    }

    return armors;
}

Equipment Character::weapon() const
{
    return equipment("무기");
}

QList<Equipment> Character::accessories() const
{
    QList<Equipment> accessories;

    const QStringList types = {"목걸이", "귀걸이", "귀걸이2", "반지", "반지2"};

    for (const QString &type : types) {
        accessories << equipment(type);
    }

    return accessories;
}

Equipment Character::abilityStone() const
{
    return equipment("어빌리티 스톤");
}

Equipment Character::bracelet() const
{
    return equipment("팔찌");
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

void Character::addEngrave(const QPair<QString, int> &engrave)
{
    mEngraves << engrave;
}

QList<QPair<QString, int> > Character::cards() const
{
    return mCards;
}

void Character::addCard(const QPair<QString, int> &card)
{
    mCards << card;
}

QList<Collectible> Character::collectibles() const
{
    return mCollectibles;
}

void Character::addCollectible(const Collectible &collectible)
{
    mCollectibles << collectible;
}

QList<Profile> Character::siblings() const
{
    return mSiblings;
}

void Character::addSiblings(const Profile &sibling)
{
    mSiblings << sibling;
}
