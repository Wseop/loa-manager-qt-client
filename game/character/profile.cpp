#include "profile.h"

Profile::Profile()
{

}

QString Profile::getServerName() const
{
    return mServerName;
}

void Profile::setServerName(const QString &serverName)
{
    mServerName = serverName;
}

QString Profile::getCharacterName() const
{
    return mCharacterName;
}

void Profile::setCharacterName(const QString &characterName)
{
    mCharacterName = characterName;
}

int Profile::getCharacterLevel() const
{
    return mCharacterLevel;
}

void Profile::setCharacterLevel(int characterLevel)
{
    mCharacterLevel = characterLevel;
}

QString Profile::getCharacterClass() const
{
    return mCharacterClass;
}

void Profile::setCharacterClass(const QString &characterClass)
{
    mCharacterClass = characterClass;
}

double Profile::getItemLevel() const
{
    return mItemLevel;
}

void Profile::setItemLevel(double itemLevel)
{
    mItemLevel = itemLevel;
}

int Profile::getExpeditionLevel() const
{
    return mExpeditionLevel;
}

void Profile::setExpeditionLevel(int expeditionLevel)
{
    mExpeditionLevel = expeditionLevel;
}

QString Profile::getTitle() const
{
    return mTitle;
}

void Profile::setTitle(const QString &title)
{
    mTitle = title;
}

QString Profile::getGuildName() const
{
    return mGuildName;
}

void Profile::setGuildName(const QString &guildName)
{
    mGuildName = guildName;
}

int Profile::getUsingSkillPoint() const
{
    return mUsingSkillPoint;
}

void Profile::setUsingSkillPoint(int usingSkillPoint)
{
    mUsingSkillPoint = usingSkillPoint;
}

int Profile::getTotalSkillPoint() const
{
    return mTotalSkillPoint;
}

void Profile::setTotalSkillPoint(int totalSkillPoint)
{
    mTotalSkillPoint = totalSkillPoint;
}

QHash<Ability, int> Profile::getAbility() const
{
    return mAbility;
}

void Profile::addAbility(Ability ability, int value)
{
    mAbility[ability] = value;
}

int Profile::getAttack() const
{
    return mAttack;
}

void Profile::setAttack(int attack)
{
    mAttack = attack;
}

int Profile::getMaxHp() const
{
    return mMaxHp;
}

void Profile::setMaxHp(int maxHp)
{
    mMaxHp = maxHp;
}
