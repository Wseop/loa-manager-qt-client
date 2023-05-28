#ifndef SKILL_H
#define SKILL_H

#include "game/skill/tripod.h"

#include <QString>
#include <QHash>

struct Skill {
    QString skillName;
    int skillCode = -1;
    QString iconPath;
    bool isCounter = false;
    QList<Tripod> tripods;

    int skillLevel = -1;
};

#endif // SKILL_H
