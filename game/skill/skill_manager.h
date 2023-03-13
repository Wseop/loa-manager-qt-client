#ifndef SKILLMANAGER_H
#define SKILLMANAGER_H

#include "game/skill/skill.h"

#include <QHash>

class SkillManager
{
private:
    SkillManager();
    ~SkillManager();

    void initializeSkillList();

public:
    static SkillManager *getInstance();
    static void destroyInstance();

public:
    QStringList skillNames(const QString &_class) const;
    QHash<QString, Skill> skills(const QString &_class) const;

private:
    static SkillManager *m_pInstance;

private:
    QHash<QString, QStringList> m_skillNames;
    QHash<QString, QHash<QString, Skill>> m_skills;
};

#endif // SKILLMANAGER_H
