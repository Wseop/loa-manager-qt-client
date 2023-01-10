#include "item.h"

Item::Item(ItemType type) :
    m_grade(ItemGrade::Size),
    m_type(type)
{

}

const QString& Item::getName() const
{
    return m_name;
}

const ItemGrade& Item::getGrade() const
{
    return m_grade;
}

const QString& Item::getIconPath() const
{
    return m_iconPath;
}

const ItemType& Item::getType() const
{
    return m_type;
}

void Item::setName(const QString& name)
{
    m_name = name;
}

void Item::setGrade(ItemGrade grade)
{
    m_grade = grade;
}

void Item::setIconPath(const QString& iconPath)
{
    m_iconPath = iconPath;
}
