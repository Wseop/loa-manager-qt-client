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
    static SkillManager *mpInstance;

private:
    QHash<QString, QStringList> mSkillNames;
    QHash<QString, QHash<QString, Skill>> mSkills;
};

#endif // SKILLMANAGER_H
