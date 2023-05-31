#ifndef SETTINGS_SKILL_H
#define SETTINGS_SKILL_H

#include <QHash>
#include <QString>

struct SkillUsage {
    int count = -1;
    QHash<int, int> levels;
    QHash<QString, int> tripods;
    QList<QPair<QString, int>> runes;
};

struct SettingSkill {
    int count = -1;
    QList<QPair<QString, SkillUsage>> skillUsages;
};

struct SettingsSkill {
    int count = -1;
    QHash<QString, SettingSkill> settings;
};

#endif // SETTINGS_SKILL_H
