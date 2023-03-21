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
    void initializeProfileLayout();
    void addProfileInfo(const Profile *pProfile);
    void addSiblingSelector(const QList<Profile*> &siblings);
    void addAbilityInfo(const QHash<Ability, int> &abilities);
    void addEngraveInfo(const Engrave *pEngrave);
    void addCardInfo(const Card *pCard);

    void initializeEquipLayout();
    void addArmorInfo(const QList<Armor*> &armors);
    void addWeaponInfo(const Weapon *pWeapon);
    void addElixirInfo(const QList<Armor*> &armors);

    void initializeAccessoryLayout();
    void addAccessoryInfo(const QList<Accessory*> &accessories);
    void addAbilityStoneInfo(const AbilityStone *pAbilityStone);
    void addBraceletInfo(const Bracelet *pBracelet);

    void initializeGemLayout();
    void addGemInfo(const QList<Gem*> &gems);

    void initializeSkillLayout();
    void addSkillInfo(const QList<Skill*> &skills);

    void addHLine(QLayout *pLayout);
    void addLayoutTitle(const QString &title, QLayout *pLayout);

private:
    Ui::CharacterInfo *ui;

    Character *mpCharacter;

    QList<QWidget *> mWidgets;
    QList<QLayout *> mLayouts;
};

#endif // CHARACTER_INFO_H
