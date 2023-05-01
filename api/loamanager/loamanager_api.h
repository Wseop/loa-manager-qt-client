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
    GetCharacterSettings,
    PostCharacterSetting,
    GetCharacterSkills,
    PostCharacterSkill
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

struct SkillSetting
{
    struct SkillData {
        QString skillName;
        QStringList tripodsNames;
        QString runeName;
    };

    QString characterName;
    QString className;
    QStringList classEngraves;
    QList<SkillData> skills;
};

#endif // LOAMANAGER_API_H
