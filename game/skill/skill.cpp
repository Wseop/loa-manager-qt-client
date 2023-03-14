#include "skill.h"
#include "game/item/rune.h"

Skill::Skill() :
    mpRune(nullptr)
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
