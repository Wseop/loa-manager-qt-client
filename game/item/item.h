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
    ItemType m_itemType;
    QString m_itemName;
    QString m_iconPath;
    ItemGrade m_itemGrade;
};

#endif // ITEM_H
