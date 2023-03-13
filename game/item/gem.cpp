#include "gem.h"

Gem::Gem(GemType gemType) :
    Item(ItemType::Gem),
    mGemType(gemType)
{

}

GemType Gem::gemType() const
{
    return mGemType;
}

int Gem::gemLevel() const
{
    return mGemLevel;
}

void Gem::setGemLevel(int newGemLevel)
{
    mGemLevel = newGemLevel;
}

QString Gem::skillName() const
{
    return mSkillName;
}

void Gem::setSkillName(const QString &newSkillName)
{
    mSkillName = newSkillName;
}
