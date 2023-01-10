#include "profile.h"

Profile::Profile() :
    m_class(Class::Size),
    m_characterLevel(0),
    m_expLevel(0),
    m_itemLevel(0)
{

}

const Class& Profile::getClass() const
{
    return m_class;
}

const int& Profile::getCharacterLevel() const
{
    return m_characterLevel;
}

const QString& Profile::getCharacterName() const
{
    return m_characterName;
}

const QString& Profile::getServer() const
{
    return m_server;
}

const QString& Profile::getTitle() const
{
    return m_title;
}

const QString& Profile::getGuild() const
{
    return m_guild;
}

const int& Profile::getExpLevel() const
{
    return m_expLevel;
}

const double& Profile::getItemLevel() const
{
    return m_itemLevel;
}

const QMap<Ability, int>& Profile::getAbilities() const
{
    return m_abilities;
}

void Profile::setClass(Class cls)
{
    m_class = cls;
}

void Profile::setCharacterLevel(int characterLevel)
{
    m_characterLevel = characterLevel;
}

void Profile::setCharacterName(const QString& characterName)
{
    m_characterName = characterName;
}

void Profile::setServer(const QString& server)
{
    m_server = server;
}

void Profile::setTitle(const QString& title)
{
    m_title = title;
}

void Profile::setGuild(const QString& guild)
{
    m_guild = guild;
}

void Profile::setExpLevel(int expLevel)
{
    m_expLevel = expLevel;
}

void Profile::setItemLevel(double itemLevel)
{
    m_itemLevel = itemLevel;
}

void Profile::addAbility(Ability ability, int value)
{
    m_abilities[ability] = value;
}


