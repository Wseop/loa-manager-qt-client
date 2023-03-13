#ifndef SKILL_H
#define SKILL_H

#include "game/skill/tripod.h"

#include <QString>
#include <QList>

class Rune;

class Skill
{
public:
    Skill();
    ~Skill();

    QString skillName() const;
    void setSkillName(const QString &newSkillName);

    int skillCode() const;
    void setSkillCode(int newSkillCode);

    QString iconPath() const;
    void setIconPath(const QString &newIconPath);

    QList<Tripod> tripods() const;
    void addTripod(const Tripod &tripod);

    int skillLevel() const;
    void setSkillLevel(int newSkillLevel);

    Rune *rune() const;
    void setRune(Rune *pNewRune);

private:
    QString mSkillName;
    int mSkillCode;
    QString mIconPath;
    QList<Tripod> mTripods;

    int mSkillLevel;
    Rune *mpRune;
};

#endif // SKILL_H
