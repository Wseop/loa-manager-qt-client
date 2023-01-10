#include "skillrune.h"

SkillRune::SkillRune(QString name, ItemGrade grade, QString iconPath) :
    m_name(name),
    m_grade(grade),
    m_iconPath(iconPath)
{

}

const QString& SkillRune::getName() const
{
    return m_name;
}

const ItemGrade& SkillRune::getGrade() const
{
    return m_grade;
}

const QString& SkillRune::getIconPath() const
{
    return m_iconPath;
}
