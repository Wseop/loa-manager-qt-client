#ifndef LOAMANAGER_API_H
#define LOAMANAGER_API_H

#include <QString>
#include <QStringList>

enum class LoamanagerApi
{
    Admin,
    ApiKey,
    GetRewardChaos,
    PostRewardChaos,
    GetRewardGuardian,
    PostRewardGuardian,
    GetCharacterSetting,
    PostCharacterSetting
};

struct Reward
{
    QString level;
    int count;
    QStringList items;
    QList<int> itemCounts;
};

struct CharacterSetting
{
    QString characterName;
    QString className;
    double itemLevel;
    QString itemSet;
    QString engrave;
    QString engraveLevel;
    QString ability;
    QString elixir;
};

#endif // LOAMANAGER_API_H
