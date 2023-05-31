#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include "game/item/item_grade.h"

#include <QString>
#include <QHash>

struct Equipment {
    QString type;
    QString name;
    QString iconPath;
    ItemGrade itemGrade;
    int quality = -1;
    int itemLevel = -1;
    QPair<QString, int> itemSet;    // {setName, setLevel}
    QHash<QString, int> elixirs;
    QHash<QString, int> abilities;
    QHash<QString, int> engraves;
    QStringList braceletEffects;
    bool isElla = false;
};

#endif // EQUIPMENT_H
