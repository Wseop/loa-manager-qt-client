#include "character.h"
#include "game_data/character/engrave/engrave.h"
#include "game_data/character/card/card.h"
#include "game_data/character/profile/profile.h"
#include "game_data/character/item/equip.h"
#include "game_data/character/item/accessory.h"
#include "game_data/character/item/abilitystone.h"
#include "game_data/character/item/bracelet.h"
#include "game_data/character/item/gem.h"
#include "game_data/character/skill/skill.h"
#include <QDebug>

Character::Character() :
    m_pProfile(nullptr),
    m_pWeapon(nullptr), m_pHead(nullptr),
    m_pTop(nullptr), m_pBottom(nullptr),
    m_pHand(nullptr), m_pShoulder(nullptr),
    m_pNeck(nullptr),
    m_pEar1(nullptr), m_pEar2(nullptr),
    m_pRing1(nullptr), m_pRing2(nullptr),
    m_pStone(nullptr), m_pBracelet(nullptr),
    m_pEngrave(new Engrave()),
    m_earIndex(0),
    m_ringIndex(0)
{

}

Character::~Character()
{
    if (m_pProfile != nullptr)
        delete m_pProfile;
    if (m_pWeapon != nullptr)
        delete m_pWeapon;
    if (m_pHead != nullptr)
        delete m_pHead;
    if (m_pTop != nullptr)
        delete m_pTop;
    if (m_pBottom != nullptr)
        delete m_pBottom;
    if (m_pHand != nullptr)
        delete m_pHand;
    if (m_pShoulder != nullptr)
        delete m_pShoulder;
    if (m_pNeck != nullptr)
        delete m_pNeck;
    if (m_pEar1 != nullptr)
        delete m_pEar1;
    if (m_pEar2 != nullptr)
        delete m_pEar2;
    if (m_pRing1 != nullptr)
        delete m_pRing1;
    if (m_pRing2 != nullptr)
        delete m_pRing2;
    if (m_pStone != nullptr)
        delete m_pStone;
    if (m_pBracelet != nullptr)
        delete m_pBracelet;
    if (m_pEngrave != nullptr)
        delete m_pEngrave;
    for (Gem* pGem : m_gems)
        delete pGem;
    m_gems.clear();
    for (Card* pCard : m_cards)
        delete pCard;
    m_cards.clear();
    for (Skill* pSkill : m_skills)
        delete pSkill;
    m_skills.clear();
}

const QList<Other>& Character::getOthers() const
{
    return m_others;
}

const Profile* Character::getProfile() const
{
    return m_pProfile;
}

const Equip* Character::getEquip(ItemType type) const
{
    switch (type)
    {
    case ItemType::무기:
        return m_pWeapon;
    case ItemType::투구:
        return m_pHead;
    case ItemType::상의:
        return m_pTop;
    case ItemType::하의:
        return m_pBottom;
    case ItemType::장갑:
        return m_pHand;
    case ItemType::어깨:
        return m_pShoulder;
    default:
        return nullptr;
    }
}

const Accessory* Character::getAccessory(ItemType type, int index) const
{
    switch (type)
    {
    case ItemType::목걸이:
        return m_pNeck;
    case ItemType::귀걸이:
        if (index == 0)
            return m_pEar1;
        else if (index == 1)
            return m_pEar2;
        else
            return nullptr;
    case ItemType::반지:
        if (index == 0)
            return m_pRing1;
        else if (index == 1)
            return m_pRing2;
        else
            return nullptr;
    default:
        return nullptr;
    }
}

const AbilityStone* Character::getAbilityStone() const
{
    return m_pStone;
}

const Bracelet* Character::getBracelet() const
{
    return m_pBracelet;
}

const QList<Gem*>& Character::getGems() const
{
    return m_gems;
}

const Engrave* Character::getEngrave() const
{
    return m_pEngrave;
}

const QList<Card*>& Character::getCards() const
{
    return m_cards;
}

const QList<Skill*>& Character::getSkills() const
{
    return m_skills;
}

void Character::addOther(const Other& other)
{
    m_others.append(other);
}

void Character::setProfile(Profile* profile)
{
    if (m_pProfile != nullptr)
        delete m_pProfile;
    m_pProfile = profile;
}

void Character::setEquip(ItemType type, Equip* equip)
{
    switch (type)
    {
    case ItemType::무기:
        if (m_pWeapon != nullptr)
            delete m_pWeapon;
        m_pWeapon = equip;
        break;
    case ItemType::투구:
        if (m_pHead != nullptr)
            delete m_pHead;
        m_pHead = equip;
        break;
    case ItemType::상의:
        if (m_pTop != nullptr)
            delete m_pTop;
        m_pTop = equip;
        break;
    case ItemType::하의:
        if (m_pBottom != nullptr)
            delete m_pBottom;
        m_pBottom = equip;
        break;
    case ItemType::장갑:
        if (m_pHand != nullptr)
            delete m_pHand;
        m_pHand = equip;
        break;
    case ItemType::어깨:
        if (m_pShoulder != nullptr)
            delete m_pShoulder;
        m_pShoulder = equip;
        break;
    default:
        qDebug() << Q_FUNC_INFO << ": Invalid type";
        return;
    }
}

void Character::setAccessory(ItemType type, Accessory* accessory)
{
    switch (type)
    {
    case ItemType::목걸이:
        if (m_pNeck != nullptr)
            delete m_pNeck;
        m_pNeck = accessory;
        break;
    case ItemType::귀걸이:
        if (m_earIndex == 0)
        {
            if (m_pEar1 != nullptr)
                delete m_pEar1;
            m_pEar1 = accessory;
            m_earIndex++;
        }
        else if (m_earIndex == 1)
        {
            if (m_pEar2 != nullptr)
                delete m_pEar2;
            m_pEar2 = accessory;
            m_earIndex++;
        }
        break;
    case ItemType::반지:
        if (m_ringIndex == 0)
        {
            if (m_pRing1 != nullptr)
                delete m_pRing1;
            m_pRing1 = accessory;
            m_ringIndex++;
        }
        else if (m_ringIndex == 1)
        {
            if (m_pRing2 != nullptr)
                delete m_pRing2;
            m_pRing2 = accessory;
            m_ringIndex++;
        }
        break;
    default:
        qDebug() << Q_FUNC_INFO << ": Invalid type";
        return;
    }
}

void Character::setAbilityStone(AbilityStone* stone)
{
    if (m_pStone != nullptr)
        delete m_pStone;
    m_pStone = stone;
}

void Character::setBracelet(Bracelet* bracelet)
{
    if (m_pBracelet != nullptr)
        delete m_pBracelet;
    m_pBracelet = bracelet;
}

void Character::addGem(Gem* gem)
{
    m_gems.append(gem);
}

void Character::setEngrave(Engrave* engrave)
{
    if (m_pEngrave != nullptr)
        delete m_pEngrave;
    m_pEngrave = engrave;
}

void Character::addCard(Card* card)
{
    m_cards.append(card);
}

void Character::addSkill(Skill* skill)
{
    m_skills.append(skill);
}
