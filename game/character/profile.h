#ifndef PROFILE_H
#define PROFILE_H

#include "game/character/ability.h"

#include <QString>
#include <QHash>

class Profile
{
public:
    Profile();

    QString serverName() const;
    void setServerName(const QString &newServerName);

    QString characterName() const;
    void setCharacterName(const QString &newCharacterName);

    int characterLevel() const;
    void setCharacterLevel(int newCharacterLevel);

    QString characterClass() const;
    void setCharacterClass(const QString &newCharacterClass);

    double itemLevel() const;
    void setItemLevel(double newItemLevel);

    int expeditionLevel() const;
    void setExpeditionLevel(int newExpeditionLevel);

    QString title() const;
    void setTitle(const QString &newTitle);

    QString guildName() const;
    void setGuildName(const QString &newGuildName);

    int totalSkillPoint() const;
    void setTotalSkillPoint(int newTotalSkillPoint);

    QHash<Ability, int> ability() const;
    void addAbility(Ability ability, int value);

    int maxHp() const;
    void setMaxHp(int newMaxHp);

private:
    QString mServerName;
    QString mCharacterName;
    int mCharacterLevel;
    QString mCharacterClass;
    double mItemLevel;

    int mExpeditionLevel;
    QString mTitle;
    QString mGuildName;
    int mTotalSkillPoint;
    QHash<Ability, int> mAbility;
    int mMaxHp;
};

#endif // PROFILE_H
