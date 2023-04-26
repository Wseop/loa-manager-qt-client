#ifndef RESPONSEPARSER_H
#define RESPONSEPARSER_H

#include "api/lostark/response_auction.h"
#include "api/lostark/response_market.h"
#include "api/loamanager/loamanager_api.h"
#include "game/character/character.h"

#include <QJsonDocument>

class ResponseParser
{
public:
    // LostarkApi
    static void parseSibling(QVariant response, Character *pCharacter);
    static void parseProfile(QVariant response, Character *pCharacter);
    static void parseEquipment(QVariant response, Character *pCharacter);
    static void parseSkill(QVariant response, Character *pCharacter);
    static void parseEngrave(QVariant response, Character *pCharacter);
    static void parseCard(QVariant response, Character *pCharacter);
    static void parseGem(QVariant response, Character *pCharacter);
    static void parseCollectible(QVariant response, Character *pCharacter);

    static ResponseAuction parseAuctionItem(QJsonDocument response);
    static ResponseMarket parseMarketItem(QJsonDocument response);

    // LoaManagerApi
    static QList<Reward> parseRewards(QJsonDocument response);
    static CharacterSetting parseCharacterSetting(QJsonDocument response);
    static QList<CharacterSetting> parseCharacterSettings(QJsonDocument response);
    static SkillSetting parseSkillSetting(QJsonDocument response);
    static QList<SkillSetting> parseSkillSettings(QJsonDocument response);

private:
    static void parseItemInfo(const QJsonObject &itemInfo, Item *pItem);
    static void parseItemTitle(const QJsonObject &itemTitle, Item *pItem);
    static void parseItemPartBox(const QJsonObject &itemPartBox, Item *pItem);
    static void parseSingleTextBox(const QString &singleTextBox, Item *pItem);
    static void parseIndentStringGroup(const QJsonObject &indentStringGroup, Item *pItem);

    static void parseItemSet(const QString &element001, const ItemType &type, Item *pItem);
    static void parseItemAdditional(const QString &element001, const ItemType &type, Item *pItem);
    static void parseItemBracelet(const QString &element001, const ItemType &type, Item *pItem);
    static void parseItemElixir(const QJsonObject &contentStr, const QStringList &keys, Item *pItem);
    static void parseItemEngrave(const QJsonObject &contentStr, const QStringList &keys, Item *pItem);

    static Weapon *parseWeapon(const QJsonObject &object);
    static Armor *parseArmor(const QJsonObject &object);
    static Accessory *parseAccessory(const QJsonObject &object);
    static AbilityStone *parseAbilityStone(const QJsonObject &object);
    static Bracelet *parseBracelet(const QJsonObject &object);
};

#endif // RESPONSEPARSER_H
