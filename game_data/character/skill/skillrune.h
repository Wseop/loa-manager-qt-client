#ifndef SKILLRUNE_H
#define SKILLRUNE_H

#include "game_data/character/item/enum/item_grade.h"
#include <QString>

class SkillRune
{
public:
    SkillRune(QString name, ItemGrade grade, QString iconPath);

    const QString& getName() const;
    const ItemGrade& getGrade() const;
    const QString& getIconPath() const;

private:
    QString m_name;
    ItemGrade m_grade;
    QString m_iconPath;
};

#endif // SKILLRUNE_H
