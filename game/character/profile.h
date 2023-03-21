#ifndef PROFILE_H
#define PROFILE_H

#include "game/character/ability.h"

#include <QString>
#include <QHash>

class Profile
{
public:
    Profile();

    QString getServerName() const;
    void setServerName(const QString &serverName);

    QString getCharacterName() const;
    void setCharacterName(const QString &characterName);

    int getCharacterLevel() const;
    void setCharacterLevel(int characterLevel);

    QString getCharacterClass() const;
    void setCharacterClass(const QString &characterClass);

    double getItemLevel() const;
    void setItemLevel(double itemLevel);

    int getExpeditionLevel() const;
    void setExpeditionLevel(int expeditionLevel);

    QString getTitle() const;
    void setTitle(const QString &title);

    QString getGuildName() const;
    void setGuildName(const QString &guildName);

    int getUsingSkillPoint() const;
    void setUsingSkillPoint(int usingSkillPoint);

    int getTotalSkillPoint() const;
    void setTotalSkillPoint(int totalSkillPoint);

    QHash<Ability, int> getAbility() const;
    void addAbility(Ability ability, int value);

    int getAttack() const;
    void setAttack(int attack);

    int getMaxHp() const;
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
    int mUsingSkillPoint;
    int mTotalSkillPoint;
    QHash<Ability, int> mAbility;
    int mAttack;
    int mMaxHp;
};

#endif // PROFILE_H
