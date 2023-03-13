#include "profile.h"

Profile::Profile()
{

}

QString Profile::serverName() const
{
    return mServerName;
}

void Profile::setServerName(const QString &newServerName)
{
    mServerName = newServerName;
}

QString Profile::characterName() const
{
    return mCharacterName;
}

void Profile::setCharacterName(const QString &newCharacterName)
{
    mCharacterName = newCharacterName;
}

int Profile::characterLevel() const
{
    return mCharacterLevel;
}

void Profile::setCharacterLevel(int newCharacterLevel)
{
    mCharacterLevel = newCharacterLevel;
}

QString Profile::characterClass() const
{
    return mCharacterClass;
}

void Profile::setCharacterClass(const QString &newCharacterClass)
{
    mCharacterClass = newCharacterClass;
}

double Profile::itemLevel() const
{
    return mItemLevel;
}

void Profile::setItemLevel(double newItemLevel)
{
    mItemLevel = newItemLevel;
}

int Profile::expeditionLevel() const
{
    return mExpeditionLevel;
}

void Profile::setExpeditionLevel(int newExpeditionLevel)
{
    mExpeditionLevel = newExpeditionLevel;
}

QString Profile::title() const
{
    return mTitle;
}

void Profile::setTitle(const QString &newTitle)
{
    mTitle = newTitle;
}

QString Profile::guildName() const
{
    return mGuildName;
}

void Profile::setGuildName(const QString &newGuildName)
{
    mGuildName = newGuildName;
}

int Profile::totalSkillPoint() const
{
    return mTotalSkillPoint;
}

void Profile::setTotalSkillPoint(int newTotalSkillPoint)
{
    mTotalSkillPoint = newTotalSkillPoint;
}

QHash<Ability, int> Profile::ability() const
{
    return mAbility;
}

void Profile::addAbility(Ability ability, int value)
{
    mAbility[ability] = value;
}

int Profile::maxHp() const
{
    return mMaxHp;
}

void Profile::setMaxHp(int newMaxHp)
{
    mMaxHp = newMaxHp;
}
