#ifndef ITEM_H
#define ITEM_H

#include "game/item/item_type.h"
#include "game/item/item_grade.h"

#include <QString>

class Item
{
public:
    Item(ItemType itemType);

    QString itemName() const;
    void setItemName(const QString &newItemName);

    QString iconPath() const;
    void setIconPath(const QString &newIconPath);

    ItemGrade itemGrade() const;
    void setItemGrade(ItemGrade newItemGrade);

    ItemType itemType() const;

private:
    ItemType mItemType;
    QString mItemName;
    QString mIconPath;
    ItemGrade mItemGrade;
};

#endif // ITEM_H
