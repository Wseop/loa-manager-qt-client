#ifndef GEM_H
#define GEM_H

#include "game/item/item_grade.h"

#include <QString>

struct Gem {
    QString type;
    int gemLevel = -1;
    QString iconPath;
    ItemGrade itemGrade;
    QString skillName;
};

#endif // GEM_H
