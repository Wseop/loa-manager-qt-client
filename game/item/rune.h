#ifndef RUNE_H
#define RUNE_H

#include "game/item/item_grade.h"

#include <QString>

struct Rune {
    QString runeName;
    ItemGrade itemGrade;
    QString iconPath;
};

#endif // RUNE_H
