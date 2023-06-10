#ifndef SETTINGS_ARMORY_H
#define SETTINGS_ARMORY_H

#include <QString>
#include <QHash>

struct SettingCount {
    int count = -1;
    QList<QPair<QString, int>> settingCounts;
};

struct SettingsArmory {
    int count = -1;
    QHash<QString, SettingCount> settingCountMap;
};

#endif // SETTINGS_ARMORY_H
