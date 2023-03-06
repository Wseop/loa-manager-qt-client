#ifndef SKILL_H
#define SKILL_H

#include "game/skill/tripod.h"

#include <QString>
#include <QList>

class Skill
{
public:
    Skill();

    QString skillName() const;
    void setSkillName(const QString &newSkillName);

    int skillCode() const;
    void setSkillCode(int newSkillCode);

    QString iconPath() const;
    void setIconPath(const QString &newIconPath);

    QList<Tripod> tripods() const;
    void addTripod(const Tripod &tripod);

private:
    QString m_skillName;
    int m_skillCode;
    QString m_iconPath;
    QList<Tripod> m_tripods;
};

#endif // SKILL_H
