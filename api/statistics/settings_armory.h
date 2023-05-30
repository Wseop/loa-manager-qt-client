#ifndef SETTINGS_ARMORY_H
#define SETTINGS_ARMORY_H

#include <QHash>
#include <QString>

struct SettingArmory {
    int count = -1;
    QList<QPair<QString, int>> abilities;
    QList<QPair<QString, int>> engraves[3];
    QList<QPair<QString, int>> itemSets;
    QList<QPair<QString, int>> elixirs;
};

struct SettingsArmory {
    int count = -1;
    QHash<QString, SettingArmory> settings;
};

#endif // SETTINGS_ARMORY_H
