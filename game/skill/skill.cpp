#include "skill.h"
#include "game/item/rune.h"

Skill::Skill() :
    mSkillLevel(1),
    mpRune(nullptr)
{

}

Skill::Skill(const Skill &other) :
    mSkillName(other.skillName()),
    mSkillCode(other.mSkillCode),
    mIconPath(other.iconPath()),
    mbCounter(other.isCounter()),
    mTripods(other.tripods()),
    mSkillLevel(other.mSkillLevel),
    mpRune(other.rune())
{

}

Skill::~Skill()
{
    if (mpRune != nullptr)
    {
        delete mpRune;
        mpRune = nullptr;
    }
}

QString Skill::skillName() const
{
    return mSkillName;
}

void Skill::setSkillName(const QString &newSkillName)
{
    mSkillName = newSkillName;
}

int Skill::skillCode() const
{
    return mSkillCode;
}

void Skill::setSkillCode(int newSkillCode)
{
    mSkillCode = newSkillCode;
}

QString Skill::iconPath() const
{
    return mIconPath;
}

void Skill::setIconPath(const QString &newIconPath)
{
    mIconPath = newIconPath;
}

QList<Tripod> Skill::tripods() const
{
    return mTripods;
}

Tripod &Skill::tripod(int index)
{
    return mTripods[index];
}

void Skill::addTripod(const Tripod &tripod)
{
    mTripods.append(tripod);
}

int Skill::skillLevel() const
{
    return mSkillLevel;
}

void Skill::setSkillLevel(int newSkillLevel)
{
    mSkillLevel = newSkillLevel;
}

bool Skill::isCounter() const
{
    return mbCounter;
}

void Skill::setCounter(bool bCounter)
{
    mbCounter = bCounter;
}

Rune *Skill::rune() const
{
    return mpRune;
}

void Skill::setRune(Rune *pNewRune)
{
    mpRune = pNewRune;
}
