#include "profile.h"

Profile::Profile()
{

}

QString Profile::serverName() const
{
    return mServerName;
}

void Profile::setServerName(const QString &serverName)
{
    mServerName = serverName;
}

QString Profile::characterName() const
{
    return mCharacterName;
}

void Profile::setCharacterName(const QString &characterName)
{
    mCharacterName = characterName;
}

int Profile::characterLevel() const
{
    return mCharacterLevel;
}

void Profile::setCharacterLevel(int characterLevel)
{
    mCharacterLevel = characterLevel;
}

QString Profile::characterClass() const
{
    return mCharacterClass;
}

void Profile::setCharacterClass(const QString &characterClass)
{
    mCharacterClass = characterClass;
}

double Profile::itemLevel() const
{
    return mItemLevel;
}

void Profile::setItemLevel(double itemLevel)
{
    mItemLevel = itemLevel;
}

int Profile::expeditionLevel() const
{
    return mExpeditionLevel;
}

void Profile::setExpeditionLevel(int expeditionLevel)
{
    mExpeditionLevel = expeditionLevel;
}

QString Profile::title() const
{
    return mTitle;
}

void Profile::setTitle(const QString &title)
{
    mTitle = title;
}

QString Profile::guildName() const
{
    return mGuildName;
}

void Profile::setGuildName(const QString &guildName)
{
    mGuildName = guildName;
}

int Profile::totalSkillPoint() const
{
    return mTotalSkillPoint;
}

void Profile::setTotalSkillPoint(int totalSkillPoint)
{
    mTotalSkillPoint = totalSkillPoint;
}

QHash<Ability, int> Profile::ability() const
{
    return mAbility;
}

void Profile::addAbility(Ability ability, int value)
{
    mAbility[ability] = value;
}

int Profile::attack() const
{
    return mAttack;
}

void Profile::setAttack(int attack)
{
    mAttack = attack;
}

int Profile::maxHp() const
{
    return mMaxHp;
}

void Profile::setMaxHp(int maxHp)
{
    mMaxHp = maxHp;
}
