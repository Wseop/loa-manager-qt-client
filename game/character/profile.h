#ifndef PROFILE_H
#define PROFILE_H

#include <QString>
#include <QHash>

struct Profile {
    int expeditionLevel = -1;
    QString title;
    QString guildName;
    int usingSkillPoint = -1;
    int totalSkillPoint = -1;
    QHash<QString, int> stats;
    QString serverName;
    QString characterName;
    int characterLevel = -1;
    QString className;
    double itemLevel = -1;
};

#endif // PROFILE_H
