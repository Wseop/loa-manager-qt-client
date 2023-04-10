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

    Reward &operator+=(const Reward &reward)
    {
        if (this->itemCounts.size() == reward.itemCounts.size())
        {
            this->count += reward.count;

            for (int i = 0; i < this->itemCounts.size(); i++)
            {
                this->itemCounts[i] += reward.itemCounts[i];
            }
        }

        return *this;
    }
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
