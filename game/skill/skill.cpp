#include "skill.h"

Skill::Skill()
{

}

QString Skill::skillName() const
{
    return m_skillName;
}

void Skill::setSkillName(const QString &newSkillName)
{
    m_skillName = newSkillName;
}

int Skill::skillCode() const
{
    return m_skillCode;
}

void Skill::setSkillCode(int newSkillCode)
{
    m_skillCode = newSkillCode;
}

QString Skill::iconPath() const
{
    return m_iconPath;
}

void Skill::setIconPath(const QString &newIconPath)
{
    m_iconPath = newIconPath;
}

QList<Tripod> Skill::tripods() const
{
    return m_tripods;
}

void Skill::addTripod(const Tripod &tripod)
{
    m_tripods.append(tripod);
}
