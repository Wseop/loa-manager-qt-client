#include "item.h"

Item::Item(ItemType itemType)
    : mItemType(itemType)
{

}

QString Item::itemName() const
{
    return mItemName;
}

void Item::setItemName(const QString &newItemName)
{
    mItemName = newItemName;
}

QString Item::iconPath() const
{
    return mIconPath;
}

void Item::setIconPath(const QString &newIconPath)
{
    mIconPath = newIconPath;
}

ItemGrade Item::itemGrade() const
{
    return mItemGrade;
}

void Item::setItemGrade(ItemGrade newItemGrade)
{
    mItemGrade = newItemGrade;
}

ItemType Item::itemType() const
{
    return mItemType;
}
