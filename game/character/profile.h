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
    void setServerName(const QString &serverName);

    QString characterName() const;
    void setCharacterName(const QString &characterName);

    int characterLevel() const;
    void setCharacterLevel(int characterLevel);

    QString characterClass() const;
    void setCharacterClass(const QString &characterClass);

    double itemLevel() const;
    void setItemLevel(double itemLevel);

    int expeditionLevel() const;
    void setExpeditionLevel(int expeditionLevel);

    QString title() const;
    void setTitle(const QString &title);

    QString guildName() const;
    void setGuildName(const QString &guildName);

    int totalSkillPoint() const;
    void setTotalSkillPoint(int totalSkillPoint);

    QHash<Ability, int> ability() const;
    void addAbility(Ability ability, int value);

    int attack() const;
    void setAttack(int attack);

    int maxHp() const;
    void setMaxHp(int maxHp);

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
    int mAttack;
    int mMaxHp;
};

#endif // PROFILE_H
