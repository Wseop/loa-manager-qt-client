#include "skill.h"

Skill::Skill() :
    m_level(0),
    m_pRune(nullptr)
{

}

Skill::~Skill()
{
    if (m_pRune != nullptr)
    {
        delete m_pRune;
        m_pRune = nullptr;
    }
}

const QString& Skill::getName() const
{
    return m_name;
}

const QString& Skill::getIconPath() const
{
    return m_iconPath;
}

const int& Skill::getLevel() const
{
    return m_level;
}

const QList<Tripod>& Skill::getTripods() const
{
    return m_tripods;
}

const SkillRune* Skill::getRune() const
{
    return m_pRune;
}

void Skill::setName(const QString& name)
{
    m_name = name;
}

void Skill::setIconPath(const QString& iconPath)
{
    m_iconPath = iconPath;
}

void Skill::setLevel(int level)
{
    m_level = level;
}

void Skill::addTripod(const Tripod& tripod)
{
    m_tripods.append(tripod);
}

void Skill::setRune(SkillRune* pRune)
{
    m_pRune = pRune;
}

