#ifndef SKILL_H
#define SKILL_H

#include <QString>
#include <QList>
#include "game_data/character/skill/skillrune.h"

struct Tripod
{
    QString name;
    int tier;
    int level;
};

static inline QString tripodColorCode(int tripodTier)
{
    switch (tripodTier)
    {
    case 0:
        return "#31ABD9";
    case 1:
        return "#7EC809";
    case 2:
        return "#FFB000";
    default:
        return "#000000";
    }
}

class Skill
{
public:
    Skill();
    ~Skill();

    const QString& getName() const;
    const QString& getIconPath() const;
    const int& getLevel() const;
    const QList<Tripod>& getTripods() const;
    const SkillRune* getRune() const;

    void setName(const QString& name);
    void setIconPath(const QString& iconPath);
    void setLevel(int level);
    void addTripod(const Tripod& tripod);
    void setRune(SkillRune* pRune);

private:
    QString m_name;
    QString m_iconPath;
    int m_level;
    QList<Tripod> m_tripods;
    SkillRune* m_pRune;
};

#endif // SKILL_H
