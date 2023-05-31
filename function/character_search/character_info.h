#ifndef CHARACTER_INFO_H
#define CHARACTER_INFO_H

#include "game/character/character.h"

#include <QWidget>

namespace Ui {
class CharacterInfo;
}

class CharacterInfo : public QWidget
{
    Q_OBJECT

public:
    CharacterInfo(class Character *pCharacter);
    ~CharacterInfo();

private:
    void initializeProfileLayout(const Character *pCharacter);
    void addSiblingSelector(const QList<Profile> &siblings, const QString &characterName);
    void addProfileInfo(const Profile &profile);
    void addAbilityInfo(const QHash<QString, int> &abilities);
    void addEngraveInfo(const QList<QPair<QString, int>> &engraves);
    void addCardInfo(const QList<QPair<QString, int>> &cards);
    void addCollectibleInfo(const QList<Collectible> &collectibles);

    void initializeEquipLayout(const Character *pCharacter);
    void addArmorInfo(const QList<Equipment> &armors);
    void addWeaponInfo(const Equipment &weapon);
    void addElixirInfo(const QList<Equipment> &armors);

    void initializeAccessoryLayout(const Character *pCharacter);
    void addAccessoryInfo(const QList<Equipment> &accessories);
    void addAbilityStoneInfo(const Equipment &abilityStone);
    void addBraceletInfo(const Equipment &bracelet);

    void initializeGemLayout(const QList<Gem> &gems);
    void addGemInfo(const QList<Gem> &gems);
    void addGemLevelAvgInfo(const QList<Gem> &gems);

    void initializeSkillLayout(const QList<Skill> &skills, int usingSkillPoint, int totalSkillPoint);
    void addTripodLevelInfo(const QList<Skill> &skills);
    void addSkillInfo(const QString &title, const QList<Skill> &skills);

    void addHLine(QLayout *pLayout);
    void addLayoutTitle(const QString &title, QLayout *pLayout);

private:
    Ui::CharacterInfo *ui;
};

#endif // CHARACTER_INFO_H
