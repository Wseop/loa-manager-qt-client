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
    static SkillManager* getInstance();
    static void destroyInstance();

public:
    QList<Skill> skillList(const QString& _class) const;

private:
    static SkillManager* m_pInstance;

private:
    QHash<QString, QList<Skill>> m_skillList;
};

#endif // SKILLMANAGER_H
