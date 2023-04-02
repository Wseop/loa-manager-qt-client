#ifndef LOAMANAGER_API_H
#define LOAMANAGER_API_H

#include <QString>

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
