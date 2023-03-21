#include "character.h"

Character::Character() :
    mpProfile(nullptr),
    mpWeapon(nullptr),
    mArmors(static_cast<int>(ArmorPart::size), nullptr),
    mAccessories(5, nullptr),
    mpAbilityStone(nullptr),
    mpBracelet(nullptr),
    mpEngrave(nullptr),
    mpCard(nullptr)
{

}

Character::~Character()
{
    for (Profile *pProfile : mSiblings)
        delete pProfile;
    mSiblings.clear();

    if (mpProfile != nullptr)
    {
        delete mpProfile;
        mpProfile = nullptr;
    }

    if (mpWeapon != nullptr)
    {
        delete mpWeapon;
        mpWeapon = nullptr;
    }

    for (Armor *pArmor : mArmors)
    {
        if (pArmor != nullptr)
        {
            delete pArmor;
            pArmor = nullptr;
        }
    }

    for (Accessory *pAccessory : mAccessories)
    {
        if (pAccessory != nullptr)
        {
            delete pAccessory;
            pAccessory = nullptr;
        }
    }

    if (mpAbilityStone != nullptr)
    {
        delete mpAbilityStone;
        mpAbilityStone = nullptr;
    }

    if (mpBracelet != nullptr)
    {
        delete mpBracelet;
        mpBracelet = nullptr;
    }

    if (mpEngrave != nullptr)
    {
        delete mpEngrave;
        mpEngrave = nullptr;
    }

    if (mpCard != nullptr)
    {
        delete mpCard;
        mpCard = nullptr;
    }

    for (Gem *pGem : mGems)
        delete pGem;
    mGems.clear();

    for (Skill *pSkill : mSkills)
        delete pSkill;
    mSkills.clear();
}

QList<Profile *> Character::getSiblings() const
{
    return mSiblings;
}

void Character::setSiblings(QList<Profile *> &siblings)
{
    mSiblings = siblings;
}

void Character::addSibling(Profile *pSibling)
{
    mSiblings << pSibling;
}

Profile *Character::getProfile() const
{
    return mpProfile;
}

void Character::setProfile(Profile *pProfile)
{
    if (mpProfile != nullptr)
        delete mpProfile;

    mpProfile = pProfile;
}

Weapon *Character::getWeapon() const
{
    return mpWeapon;
}

void Character::setWeapon(Weapon *pWeapon)
{
    if (mpWeapon != nullptr)
        delete mpWeapon;

    mpWeapon = pWeapon;
}

QList<Armor *> Character::getArmors() const
{
    return mArmors;
}

Armor *Character::getArmor(ArmorPart part) const
{
    if (part == ArmorPart::size)
        return nullptr;

    return mArmors[static_cast<int>(part)];
}

void Character::setArmor(Armor *pArmor)
{
    int index = static_cast<int>(pArmor->armorPart());

    if (mArmors[index] != nullptr)
        delete mArmors[index];

    mArmors[index] = pArmor;
}

QList<Accessory *> Character::getAccessories() const
{
    return mAccessories;
}

Accessory *Character::getAccessory(AccessoryPart part, int index) const
{
    if (index < 0 || index > 2)
        return nullptr;

    if (part == AccessoryPart::Necklace)
        return mAccessories[static_cast<int>(part)];
    else
        return mAccessories[static_cast<int>(part) + index];
}

void Character::setAccessory(Accessory *pAccessory, int index)
{
    if (index < 0 || index > 2)
    {
        delete pAccessory;
        return;
    }

    AccessoryPart part = pAccessory->accessoryPart();
    Accessory **dpAccessory = nullptr;

    if (part == AccessoryPart::Necklace)
        dpAccessory = &mAccessories[static_cast<int>(part)];
    else
        dpAccessory = &mAccessories[static_cast<int>(part) + index];

    if (pAccessory != nullptr)
        delete *dpAccessory;

    *dpAccessory = pAccessory;
}

AbilityStone *Character::getAbilityStone() const
{
    return mpAbilityStone;
}

void Character::setAbilityStone(AbilityStone *pAbilityStone)
{
    if (mpAbilityStone != nullptr)
        delete mpAbilityStone;

    mpAbilityStone = pAbilityStone;
}

Bracelet *Character::getBracelet() const
{
    return mpBracelet;
}

void Character::setBracelet(Bracelet *pBracelet)
{
    if (mpBracelet != nullptr)
        delete mpBracelet;

    mpBracelet = pBracelet;
}

Engrave *Character::getEngrave() const
{
    return mpEngrave;
}

void Character::setEngrave(Engrave *pEngrave)
{
    if (mpEngrave != nullptr)
        delete mpEngrave;

    mpEngrave = pEngrave;
}

Card *Character::getCard() const
{
    return mpCard;
}

void Character::setCard(Card *pCard)
{
    if (mpCard != nullptr)
        delete mpCard;

    mpCard = pCard;
}

QList<Gem *> Character::getGems() const
{
    return mGems;
}

void Character::setGems(const QList<Gem *> &gems)
{
    mGems = gems;
}

void Character::addGem(Gem *pGem)
{
    if (pGem != nullptr)
        mGems << pGem;
}

QList<Skill *> Character::getSkills() const
{
    return mSkills;
}

void Character::addSkill(Skill *pSkill)
{
    if (pSkill != nullptr)
    {
        for (auto iter = mSkills.constBegin(); iter != mSkills.constEnd(); iter++)
        {
            Skill *p = *iter;

            if (p->skillName() == pSkill->skillName())
            {
                iter = mSkills.erase(iter);
                mSkills.insert(iter, pSkill);

                delete p;
                return;
            }
        }

        mSkills << pSkill;
    }
}
