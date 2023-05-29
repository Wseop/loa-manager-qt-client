#ifndef SKILLMANAGER_H
#define SKILLMANAGER_H

#include "game/skill/skill.h"

#include <QObject>
#include <QHash>
#include <QEventLoop>

class SkillManager : public QObject
{
    Q_OBJECT

private:
    SkillManager();
    ~SkillManager();

    void initializeSkillList();

public:
    static SkillManager *getInstance();
    static void destroyInstance();

public:
    QStringList skillNames(const QString &className) const;
    QHash<QString, Skill> skills(const QString &className) const;

    Skill skill(const QString &className, const QString &skillName) const;

private:
    static SkillManager *mpInstance;

private:
    QHash<QString, QHash<QString, Skill>> mSkillMap;

    QEventLoop mEventLoop;
};

#endif // SKILLMANAGER_H
