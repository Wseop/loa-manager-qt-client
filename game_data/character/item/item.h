#ifndef ITEM_H
#define ITEM_H

#include "game_data/character/item/enum/item_grade.h"
#include "game_data/character/item/enum/item_type.h"
#include <QString>
#include <QStringList>

class Item
{
public:
    Item(ItemType type);

    const QString& getName() const;
    const ItemGrade& getGrade() const;
    const QString& getIconPath() const;
    const ItemType& getType() const;

    void setName(const QString& name);
    void setGrade(ItemGrade grade);
    void setIconPath(const QString& iconPath);

private:
    QString m_name;
    ItemGrade m_grade;
    QString m_iconPath;
    ItemType m_type;
};

#endif // ITEM_H
