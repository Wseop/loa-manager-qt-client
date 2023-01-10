#ifndef CHARACTER_H
#define CHARACTER_H

#include "game_data/character/profile/enum/class.h"
#include "game_data/character/item/enum/item_type.h"
#include <QString>
#include <QList>

class Profile;
class Equip;
class Accessory;
class AbilityStone;
class Bracelet;
class Gem;
class Engrave;
class Card;
class Skill;

struct Other
{
    QString server;
    int characterLevel;
    QString characterName;
    Class cls;
    double itemLevel;
};

class Character
{
public:
    Character();
    ~Character();

    const QList<Other>& getOthers() const;
    const Profile* getProfile() const;
    const Equip* getEquip(ItemType type) const;
    const Accessory* getAccessory(ItemType type, int index = 0) const;
    const AbilityStone* getAbilityStone() const;
    const Bracelet* getBracelet() const;
    const QList<Gem*>& getGems() const;
    const Engrave* getEngrave() const;
    const QList<Card*>& getCards() const;
    const QList<Skill*>& getSkills() const;

    void addOther(const Other& other);
    void setProfile(Profile* profile);
    void setEquip(ItemType type, Equip* equip);
    void setAccessory(ItemType type, Accessory* accessory);
    void setAbilityStone(AbilityStone* stone);
    void setBracelet(Bracelet* bracelet);
    void addGem(Gem* gem);
    void setEngrave(Engrave* engrave);
    void addCard(Card* card);
    void addSkill(Skill* skill);

private:
    QList<Other> m_others;
    Profile* m_pProfile;
    Equip* m_pWeapon;
    Equip* m_pHead;
    Equip* m_pTop;
    Equip* m_pBottom;
    Equip* m_pHand;
    Equip* m_pShoulder;
    Accessory* m_pNeck;
    Accessory* m_pEar1;
    Accessory* m_pEar2;
    Accessory* m_pRing1;
    Accessory* m_pRing2;
    AbilityStone* m_pStone;
    Bracelet* m_pBracelet;
    QList<Gem*> m_gems;
    Engrave* m_pEngrave;
    QList<Card*> m_cards;
    QList<Skill*> m_skills;

    int m_earIndex;
    int m_ringIndex;
};

#endif // CHARACTER_H
