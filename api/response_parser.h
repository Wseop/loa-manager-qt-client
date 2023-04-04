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
    static void parseSibling(QJsonDocument response, Character *pCharacter);
    static void parseProfile(QJsonDocument response, Character *pCharacter);
    static void parseEquipment(QJsonDocument response, Character *pCharacter);
    static void parseSkill(QJsonDocument response, Character *pCharacter);
    static void parseEngrave(QJsonDocument response, Character *pCharacter);
    static void parseCard(QJsonDocument response, Character *pCharacter);
    static void parseGem(QJsonDocument response, Character *pCharacter);

    static ResponseAuction parseAuctionItem(QJsonDocument response);
    static ResponseMarket parseMarketItem(QJsonDocument response);

    // LoaManagerApi
    static QList<Reward> parseRewards(QJsonDocument response);
    static CharacterSetting parseCharacterSetting(QJsonDocument response);
    static QList<CharacterSetting> parseCharacterSettings(QJsonDocument response);

private:
    static void parseItemInfo(const QJsonObject &itemInfo, Item *pItem);
    static void parseItemTitle(const QJsonObject &itemTitle, Item *pItem);
    static void parseItemPartBox(const QJsonObject &itemPartBox, Item *pItem);
    static void parseSingleTextBox(const QString &singleTextBox, Item *pItem);
    static void parseIndentStringGroup(const QJsonObject &indentStringGroup, Item *pItem);

    static Weapon *parseWeapon(const QJsonObject &object);
    static Armor *parseArmor(const QJsonObject &object);
    static Accessory *parseAccessory(const QJsonObject &object);
    static AbilityStone *parseAbilityStone(const QJsonObject &object);
    static Bracelet *parseBracelet(const QJsonObject &object);
};

#endif // RESPONSEPARSER_H
