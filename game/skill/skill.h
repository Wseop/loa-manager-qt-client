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
    Skill(const Skill &other);
    ~Skill();

    QString skillName() const;
    void setSkillName(const QString &newSkillName);

    int skillCode() const;
    void setSkillCode(int newSkillCode);

    QString iconPath() const;
    void setIconPath(const QString &newIconPath);

    QList<Tripod> tripods() const;
    Tripod &tripod(int index);
    void addTripod(const Tripod &tripod);

    int skillLevel() const;
    void setSkillLevel(int newSkillLevel);

    bool isCounter() const;
    void setCounter(bool bCounter);

    Rune *rune() const;
    void setRune(Rune *pNewRune);

private:
    QString mSkillName;
    int mSkillCode;
    QString mIconPath;
    bool mbCounter;
    QList<Tripod> mTripods;
    int mSkillLevel;
    Rune *mpRune;
};

#endif // SKILL_H
