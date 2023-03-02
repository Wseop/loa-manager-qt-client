#include "item.h"

Item::Item(ItemType itemType)
    : m_itemType(itemType)
{

}

QString Item::itemName() const
{
    return m_itemName;
}

void Item::setItemName(const QString& newItemName)
{
    m_itemName = newItemName;
}

QString Item::iconPath() const
{
    return m_iconPath;
}

void Item::setIconPath(const QString& newIconPath)
{
    m_iconPath = newIconPath;
}

ItemGrade Item::itemGrade() const
{
    return m_itemGrade;
}

void Item::setItemGrade(ItemGrade newItemGrade)
{
    m_itemGrade = newItemGrade;
}

ItemType Item::itemType() const
{
    return m_itemType;
}
