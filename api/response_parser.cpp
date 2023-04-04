#include "response_parser.h"
#include "game/skill/skill_manager.h"
#include "game/item/rune.h"

#include <QJsonObject>
#include <QJsonArray>

#include <algorithm>

void ResponseParser::parseSibling(QJsonDocument response, Character *pCharacter)
{
    // TODO. 무한 대기 개선 필요
    // Profile이 업데이트 될 때까지 대기
    while (pCharacter->getProfile() == nullptr);

    const QJsonArray &siblings = response.array();
    QList<Profile *> newSiblings;

    for (int i = 0; i < siblings.size(); i++)
    {
        const QJsonObject &sibling = siblings[i].toObject();
        const QString &siblingName = sibling.find("CharacterName")->toString();

        // 검색 대상 캐릭터는 보유 캐릭터 목록에서 제외
        if (siblingName == pCharacter->getProfile()->getCharacterName())
            continue;

        Profile *pProfile = new Profile();
        pProfile->setServerName(sibling.find("ServerName")->toString());
        pProfile->setCharacterName(siblingName);
        pProfile->setCharacterLevel(sibling.find("CharacterLevel")->toInt());
        pProfile->setCharacterClass(sibling.find("CharacterClassName")->toString());
        pProfile->setItemLevel(sibling.find("ItemAvgLevel")->toString().remove(",").toDouble());

        newSiblings << pProfile;
    }

    // 아이템 레벨을 기준으로 내림차순 정렬
    std::sort(newSiblings.begin(), newSiblings.end(), [&](Profile *a, Profile *b){
        return a->getItemLevel() > b->getItemLevel();
    });

    pCharacter->setSiblings(newSiblings);
}

void ResponseParser::parseProfile(QJsonDocument response, Character *pCharacter)
{
    const QJsonObject &profile = response.object();

    Profile *pProfile = new Profile();
    pProfile->setExpeditionLevel(profile.find("ExpeditionLevel")->toInt());
    pProfile->setTitle(profile.find("Title")->toString());
    pProfile->setGuildName(profile.find("GuildName")->toString());
    pProfile->setUsingSkillPoint(profile.find("UsingSkillPoint")->toInt());
    pProfile->setTotalSkillPoint(profile.find("TotalSkillPoint")->toInt());
    pProfile->setServerName(profile.find("ServerName")->toString());
    pProfile->setCharacterName(profile.find("CharacterName")->toString());
    pProfile->setCharacterLevel(profile.find("CharacterLevel")->toInt());
    pProfile->setCharacterClass(profile.find("CharacterClassName")->toString());
    pProfile->setItemLevel(profile.find("ItemAvgLevel")->toString().remove(",").toDouble());

    const QJsonArray &stats = profile.find("Stats")->toArray();

    for (int i = 0; i < stats.size(); i++)
    {
        const QJsonObject &stat = stats[i].toObject();
        const QString &type = stat.find("Type")->toString();
        int value = stat.find("Value")->toString().toInt();

        if (type == "최대 생명력")
            pProfile->setMaxHp(value);
        else if (type == "공격력")
            pProfile->setAttack(value);
        else
            pProfile->addAbility(qStringToAbility(type), value);
    }

    pCharacter->setProfile(pProfile);
}

void ResponseParser::parseEquipment(QJsonDocument response, Character *pCharacter)
{
    const QJsonArray &equipments = response.array();
    const QStringList armorTypes = {"투구", "상의", "하의", "장갑", "어깨"};
    int earringCount = 0;
    int ringCount = 0;

    for (int i = 0; i < equipments.size(); i++)
    {
        const QJsonObject &equipment = equipments[i].toObject();
        const QString &type = equipment.find("Type")->toString();

        if (type == "무기")
            pCharacter->setWeapon(parseWeapon(equipment));
        else if (armorTypes.contains(type))
            pCharacter->setArmor(parseArmor(equipment));
        else if (type == "목걸이")
            pCharacter->setAccessory(parseAccessory(equipment), 0);
        else if (type == "귀걸이")
            pCharacter->setAccessory(parseAccessory(equipment), earringCount++);
        else if (type == "반지")
            pCharacter->setAccessory(parseAccessory(equipment), ringCount++);
        else if (type == "어빌리티 스톤")
            pCharacter->setAbilityStone(parseAbilityStone(equipment));
        else if (type == "팔찌")
            pCharacter->setBracelet(parseBracelet(equipment));
    }
}

void ResponseParser::parseSkill(QJsonDocument response, Character *pCharacter)
{
    // TODO. 무한 대기 개선 필요
    // Profile이 업데이트 될 때까지 대기
    while (pCharacter->getProfile() == nullptr);

    // 해당 직업의 스킬 목록 생성
    const QString &characterClass = pCharacter->getProfile()->getCharacterClass();
    QHash<QString, Skill> characterSkills = SkillManager::getInstance()->skills(characterClass);

    const QJsonArray &skills = response.array();

    for (int i = 0; i < skills.size(); i++)
    {
        const QJsonObject &skill = skills[i].toObject();
        const QString &skillName = skill.find("Name")->toString();

        if (characterSkills.contains(skillName))
        {
            // 스킬 객체 생성 및 스킬 레벨 parsing
            Skill *pSkill = new Skill(characterSkills[skillName]);
            pSkill->setSkillLevel(skill.find("Level")->toInt());

            // 트라이포드 레벨, 선택 여부 parsing
            const QJsonArray &tripods = skill.find("Tripods")->toArray();

            for (int j = 0; j < tripods.size(); j++)
            {
                const QJsonObject &tripod = tripods[j].toObject();

                pSkill->tripod(j).setTripodLevel(tripod.find("Level")->toInt());
                pSkill->tripod(j).setSelected(tripod.find("IsSelected")->toBool());
            }

            // 룬 parsing
            const QJsonObject &rune = skill.find("Rune")->toObject();

            if (!rune.isEmpty())
            {
                Rune *pRune = new Rune();
                pRune->setItemName(rune.find("Name")->toString());
                pRune->setIconPath(rune.find("Icon")->toString());
                pRune->setItemGrade(qStringToItemGrade(rune.find("Grade")->toString()));

                pSkill->setRune(pRune);
            }

            pCharacter->addSkill(pSkill);
        }
    }
}

void ResponseParser::parseEngrave(QJsonDocument response, Character *pCharacter)
{
    const QJsonArray &engraves = response.object().find("Effects")->toArray();
    Engrave *pEngrave = new Engrave();

    for (int i = 0; i < engraves.size(); i++)
    {
        const QJsonObject &engrave = engraves[i].toObject();
        const QString &text = engrave.find("Name")->toString();

        // 각인명, 각인 레벨 parsing
        QString engraveName = text.sliced(0, text.indexOf("Lv") - 1);
        int engraveLevel = text.back().digitValue();

        if (engraveName.contains("감소"))
            pEngrave->addPenaltyLevel(engraveName, engraveLevel);
        else
            pEngrave->addEngraveLevel(engraveName, engraveLevel);
    }

    pCharacter->setEngrave(pEngrave);
}

void ResponseParser::parseCard(QJsonDocument response, Character *pCharacter)
{
    const QJsonArray &cardEffects = response.object().find("Effects")->toArray();
    Card *pCard = new Card();

    for (int i = 0; i < cardEffects.size(); i++)
    {
        const QJsonObject &cardEffect = cardEffects[i].toObject();
        const QJsonArray &items = cardEffect.find("Items")->toArray();

        if (items.size() != 0)
            pCard->addCardSet(items.last().toObject().find("Name")->toString());
    }

    pCharacter->setCard(pCard);
}

void ResponseParser::parseGem(QJsonDocument response, Character *pCharacter)
{
    const QJsonObject &gemObject = response.object();
    const QJsonArray &gems = gemObject.find("Gems")->toArray();
    const QJsonArray &effects = gemObject.find("Effects")->toArray();

    // slot 번호에 해당하는 보석의 스킬명 parsing
    QHash<int, QString> slotSkillName;

    for (int i = 0; i < effects.size(); i++)
    {
        const QJsonObject &effect = effects[i].toObject();
        int slot = effect.find("GemSlot")->toInt();
        const QString &skillName = effect.find("Name")->toString();

        slotSkillName[slot] = skillName;
    }

    // 보석 정보 parsing 및 추가
    QList<Gem*> newGems;

    for (int i = 0; i < gems.size(); i++)
    {
        const QJsonObject &gem = gems[i].toObject();
        const QString &name = gem.find("Name")->toString();

        GemType gemType;
        if (name.contains("멸화"))
            gemType = GemType::멸화;
        else if (name.contains("홍염"))
            gemType = GemType::홍염;
        else
            continue;

        Gem *pGem = new Gem(gemType);
        pGem->setIconPath(gem.find("Icon")->toString());
        pGem->setGemLevel(gem.find("Level")->toInt());
        pGem->setItemGrade(qStringToItemGrade(gem.find("Grade")->toString()));
        pGem->setSkillName(slotSkillName[gem.find("Slot")->toInt()]);

        QString gemName = QString("%1레벨 ").arg(pGem->gemLevel());
        if (gemType == GemType::멸화)
            gemName += "멸화의 보석";
        else
            gemName += "홍염의 보석";

        pGem->setItemName(gemName);

        newGems << pGem;
    }

    // 보석 레벨을 기준으로 내림차순 정렬
    std::sort(newGems.begin(), newGems.end(), [&](Gem *a, Gem *b){
        return a->gemLevel() > b->gemLevel();
    });

    pCharacter->setGems(newGems);
}

ResponseAuction ResponseParser::parseAuctionItem(QJsonDocument response)
{
    const QJsonObject &object = response.object();

    ResponseAuction responseAuction;
    responseAuction.pageNo = object.find("PageNo")->toInt();
    responseAuction.pageSize = object.find("PageSize")->toInt();
    responseAuction.totalCount = object.find("TotalCount")->toInt();

    const QJsonArray &items = object.find("Items")->toArray();

    for (int i = 0; i < items.size(); i++)
    {
        const QJsonObject &item = items[i].toObject();

        AuctionItem auctionItem;
        auctionItem.name = item.find("Name")->toString();
        auctionItem.grade = item.find("Grade")->toString();
        auctionItem.tier = item.find("Tier")->toInt();
        auctionItem.icon = item.find("Icon")->toString();
        auctionItem.quality = item.find("GradeQuality")->toInt();

        const QJsonObject &auctionInfo = item.find("AuctionInfo")->toObject();

        auctionItem.AuctionInfo.startPrice = auctionInfo.find("StartPrice")->toInt();
        auctionItem.AuctionInfo.buyPrice = auctionInfo.find("BuyPrice")->toInt();
        auctionItem.AuctionInfo.bidStartPrice = auctionInfo.find("BidStartPrice")->toInt();

        const QJsonArray &options = item.find("Options")->toArray();

        for (int i = 0; i < options.size(); i++)
        {
            const QJsonObject &option = options[i].toObject();

            AuctionItemOption auctionItemOption;
            auctionItemOption.type = option.find("Type")->toString();
            auctionItemOption.optionName = option.find("OptionName")->toString();
            auctionItemOption.optionNameTripod = option.find("OptionNameTripod")->toString();
            auctionItemOption.value = option.find("Value")->toInt();
            auctionItemOption.bPenalty = option.find("IsPenalty")->toBool();
            auctionItemOption.className = option.find("ClassName")->toString();

            auctionItem.options << auctionItemOption;
        }

        responseAuction.items << auctionItem;
    }

    return responseAuction;
}

ResponseMarket ResponseParser::parseMarketItem(QJsonDocument response)
{
    const QJsonObject &object = response.object();

    ResponseMarket responseMarket;
    responseMarket.pageNo = object.find("PageNo")->toInt();
    responseMarket.pageSize = object.find("PageSize")->toInt();
    responseMarket.totalCount = object.find("TotalCount")->toInt();

    const QJsonArray &items = object.find("Items")->toArray();

    for (int i = 0; i < items.size(); i++)
    {
        const QJsonObject &item = items[i].toObject();

        MarketItem marketItem;
        marketItem.name = item.find("Name")->toString();
        marketItem.grade = item.find("Grade")->toString();
        marketItem.icon = item.find("Icon")->toString();
        marketItem.yDayAvgPrice = item.find("YDayAvgPrice")->toDouble();
        marketItem.recentPrice = item.find("RecentPrice")->toInt();
        marketItem.currentMinPrice = item.find("CurrentMinPrice")->toInt();

        responseMarket.items << marketItem;
    }

    return responseMarket;
}

QList<Reward> ResponseParser::parseRewards(QJsonDocument response)
{
    const QJsonArray rewardDatas = response.array();

    QList<Reward> rewards;

    for (const QJsonValue &value : rewardDatas)
    {
        const QJsonObject &object = value.toObject();

        Reward reward;

        reward.level = object.find("level")->toString();
        reward.count = object.find("count")->toInt();

        QStringList keys;

        if (reward.level.back().isDigit())
            keys = {"silling", "shard", "destruction", "protection", "leapStone", "gem"};
        else
            keys = {"destruction", "protection", "leapStone"};

        for (const QString &key : keys)
        {
            reward.items << key;
            reward.itemCounts << object.find(key)->toInt();
        }

        rewards << reward;
    }

    return rewards;
}

CharacterSetting ResponseParser::parseCharacterSetting(QJsonDocument response)
{
    const QJsonObject &object = response.object();

    CharacterSetting characterSetting;

    characterSetting.characterName = object.find("characterName")->toString();
    characterSetting.className = object.find("className")->toString();
    characterSetting.itemLevel = object.find("itemLevel")->toDouble();
    characterSetting.itemSet = object.find("itemSet")->toString();
    characterSetting.engrave = object.find("engrave")->toString();
    characterSetting.engraveLevel = object.find("engraveLevel")->toString();
    characterSetting.ability = object.find("ability")->toString();

    QVariant elixir = object.find("elixir")->toVariant();

    if (elixir.isNull())
        characterSetting.elixir = "";
    else
        characterSetting.elixir = elixir.toString();

    return characterSetting;
}

QList<CharacterSetting> ResponseParser::parseCharacterSettings(QJsonDocument response)
{
    const QJsonArray &array = response.array();

    QList<CharacterSetting> characterSettings;

    for (const QJsonValue &value : array)
    {
        characterSettings << parseCharacterSetting(QJsonDocument::fromVariant(value.toVariant()));
    }

    return characterSettings;
}

void ResponseParser::parseItemInfo(const QJsonObject &itemInfo, Item *pItem)
{
    const QStringList armorParts = {"투구", "어깨", "상의", "하의", "장갑"};

    // type parsing
    const QString &type = itemInfo.find("Type")->toString();

    if (armorParts.contains(type))
        static_cast<Armor*>(pItem)->setArmorPart(static_cast<ArmorPart>(armorParts.indexOf(type)));
    else if (type == "목걸이")
        static_cast<Accessory*>(pItem)->setAccessoryPart(AccessoryPart::Necklace);
    else if (type == "귀걸이")
        static_cast<Accessory*>(pItem)->setAccessoryPart(AccessoryPart::Earring);
    else if (type == "반지")
        static_cast<Accessory*>(pItem)->setAccessoryPart(AccessoryPart::Ring);

    // name, icon, grade parsing
    pItem->setItemName(itemInfo.find("Name")->toString());
    pItem->setIconPath(itemInfo.find("Icon")->toString());
    pItem->setItemGrade(qStringToItemGrade(itemInfo.find("Grade")->toString()));

    // 강화 수치 parsing
    int reforge = 0;

    if (pItem->itemName().front() == '+')
        reforge = pItem->itemName().sliced(1, 2).toInt();
    if (reforge != 0)
    {
        if (pItem->itemType() == ItemType::Weapon)
            static_cast<Weapon*>(pItem)->setReforge(reforge);
        else if (pItem->itemType() == ItemType::Armor)
            static_cast<Armor*>(pItem)->setReforge(reforge);
    }
}

void ResponseParser::parseItemTitle(const QJsonObject &itemTitle, Item *pItem)
{
    // 품질 parsing
    int quality = itemTitle.find("qualityValue")->toInt();

    // 아이템 레벨, 티어 parsing
    const QString &leftStr2 = itemTitle.find("leftStr2")->toString();
    int itemLevelIndex = leftStr2.indexOf("아이템 레벨");
    int itemTierIndex = leftStr2.indexOf("티어");
    int itemLevel = 0;
    int itemTier = 0;

    if (itemLevelIndex != -1 && itemTierIndex != -1)
    {
        // 아이템 레벨 정수 위치로 index 설정
        int startIndex = itemLevelIndex + 7;
        int endIndex = itemTierIndex - 2;
        // 아이템 레벨 parsing
        itemLevel = leftStr2.sliced(startIndex, endIndex - startIndex).toInt();

        // 아이템 티어 정수 위치로 index 설정
        startIndex = itemTierIndex + 3;
        // 아이템 티어 parsing
        itemTier = leftStr2.sliced(startIndex, 1).toInt();
    }

    if (ItemType::Weapon == pItem->itemType())
    {
        Weapon *pWeapon = static_cast<Weapon*>(pItem);
        pWeapon->setQuality(quality);
        pWeapon->setItemLevel(itemLevel);
        pWeapon->setTier(itemTier);
    }
    else if (ItemType::Armor == pItem->itemType())
    {
        Armor *pArmor = static_cast<Armor*>(pItem);
        pArmor->setQuality(quality);
        pArmor->setItemLevel(itemLevel);
        pArmor->setTier(itemTier);
    }
    else if (ItemType::Accessory == pItem->itemType())
    {
        Accessory *pAccessory = static_cast<Accessory*>(pItem);
        pAccessory->setQuality(quality);
    }
}

void ResponseParser::parseItemPartBox(const QJsonObject &itemPartBox, Item *pItem)
{
    ItemType type = pItem->itemType();
    const QString &element000 = itemPartBox.find("Element_000")->toString();
    const QString &element001 = itemPartBox.find("Element_001")->toString();

    if (element000.contains("세트 효과 레벨"))
    {
        // 세트 효과 및 세트 레벨 parsing
        int setLevelIndex = element001.indexOf("Lv.");
        // element의 첫 2글자는 세트 효과 이름
        ItemSet itemSet = qStringToItemSet(element001.sliced(0, 2));
        // 세트 레벨 (Lv.#)에서 정수 부분 parsing
        int setLevel = element001.sliced(setLevelIndex + QString("Lv.").size(), 1).toInt();

        if (ItemType::Weapon == type)
        {
            Weapon *pWeapon = static_cast<Weapon*>(pItem);
            pWeapon->setItemSet(itemSet);
            pWeapon->setSetLevel(setLevel);
        }
        else if (ItemType::Armor == type)
        {
            Armor *pArmor = static_cast<Armor*>(pItem);
            pArmor->setItemSet(itemSet);
            pArmor->setSetLevel(setLevel);
        }
    }
    else if (element000.contains("추가 효과"))
    {
        // 특성 parsing (특성 +###, ...)
        if (ItemType::Accessory == type)
        {
            Accessory *pAccessory = static_cast<Accessory*>(pItem);
            int abilityStartIndex = 0;
            int valueStartIndex = element001.indexOf("+");
            int valueEndIndex = element001.indexOf("<BR>");

            while (valueStartIndex != -1)
            {
                const QString &ability = element001.sliced(abilityStartIndex, 2);

                valueStartIndex += QString("+").size();
                if (valueEndIndex == -1)
                    valueEndIndex = element001.size();

                int abilityValue = element001.sliced(valueStartIndex, valueEndIndex - valueStartIndex).toInt();

                pAccessory->setAbility(qStringToAbility(ability), abilityValue);

                // 다음 특성 위치로 index 설정
                abilityStartIndex = element001.indexOf(">") + QString(">").size();
                valueStartIndex = element001.indexOf("+", valueStartIndex);
                valueEndIndex = element001.indexOf("<BR>", valueEndIndex + 1);
            }
        }
    }
    else if (element000.contains("팔찌 효과"))
    {
        // 팔찌 효과 parsing
        if (ItemType::Bracelet == type)
        {
            Bracelet *pBracelet = static_cast<Bracelet*>(pItem);
            // 팔찌 효과는 </img> 로 시작
            int startIndex = element001.indexOf("</img>");
            // </img>와 이후 처음 등장하는 <BR> 사이에 팔찌 효과가 존재
            int endIndex = element001.indexOf("<BR>", startIndex);

            while (startIndex != -1)
            {
                // <BR>이 더 이상 존재하지 않으면 마지막 효과
                if (endIndex == -1)
                    endIndex = element001.size();
                startIndex += QString("</img>").size();

                QString effectStr = element001.sliced(startIndex, endIndex - startIndex);

                // 공백 제거
                if (effectStr.front() == ' ')
                    effectStr = effectStr.sliced(1, effectStr.size() - 1);

                // [ 로 시작하면 특수 효과, 아니면 일반 효과
                if (effectStr.front() == '[')
                {
                    // 특수 효과명은 <FONT> 태그로 묶여있음
                    int effectStartIndex = effectStr.indexOf(">") + QString(">").size();
                    int effectEndIndex = effectStr.indexOf("</FONT>");

                    // TODO. 특수 효과 value parsing
                    pBracelet->addSpecialEffect(effectStr.sliced(effectStartIndex, effectEndIndex - effectStartIndex), 0);
                }
                else
                {
                    // 효과명과 값 parsing 후 추가
                    int valueStartIndex = effectStr.indexOf("+") + QString("+").size();

                    if (valueStartIndex != 0)
                    {
                        const QString &effect = effectStr.sliced(0, valueStartIndex - 2);
                        int value = effectStr.sliced(valueStartIndex, effectStr.size() - valueStartIndex).toInt();

                        pBracelet->addEffect(effect, value);
                    }
                }

                // 다음 효과 탐색
                startIndex = element001.indexOf("</img>", startIndex);
                endIndex = element001.indexOf("<BR>", startIndex);
            }
        }
    }
}

void ResponseParser::parseSingleTextBox(const QString &singleTextBox, Item *pItem)
{
    if (singleTextBox.contains("엘라 부여 완료"))
        static_cast<Weapon*>(pItem)->setElla(true);
}

void ResponseParser::parseIndentStringGroup(const QJsonObject &indentStringGroup, Item *pItem)
{
    const QString &topStr = indentStringGroup.find("Element_000")->toObject().find("topStr")->toString();
    const QJsonObject &contentStr = indentStringGroup.find("Element_000")->toObject().find("contentStr")->toObject();
    const QStringList &keys = contentStr.keys();

    if (topStr.contains("엘릭서 효과") && pItem->itemType() == ItemType::Armor)
    {
        // 엘릭서 parsing
        for (const QString &key : keys)
        {
            const QString &elixirStr = contentStr.find(key)->toObject().find("contentStr")->toString();
            int partStartIndex = elixirStr.indexOf("[");
            int effectStartIndex = partStartIndex + QString("[##]</FONT> ").size();
            int levelStartIndex = elixirStr.indexOf("Lv.");
            int effectEndIndex = levelStartIndex - QString(" <FONT color='#FFFFFF'>").size();

            const QString& part = elixirStr.sliced(partStartIndex + QString("[").size(), 2);
            const QString& effect = elixirStr.sliced(effectStartIndex, effectEndIndex - effectStartIndex);
            int level = elixirStr.sliced(levelStartIndex + QString("Lv.").size(), 1).toInt();

            static_cast<Armor*>(pItem)->addElixir(part, effect, level);
        }
    }
    else if (topStr.contains("무작위 각인 효과"))
    {
        // 각인 parsing
        for (const QString &key : keys)
        {
            const QString &engrave = contentStr.find(key)->toObject().find("contentStr")->toString();
            int engraveNameStartIndex = engrave.indexOf(">") + QString(">").size();
            int engraveNameEndIndex = engrave.indexOf("</FONT>");
            int engraveValueStartIndex = engrave.indexOf("+") + QString("+").size();
            int engraveValueEndIndex = engrave.indexOf("<BR>", engraveValueStartIndex);

            const QString &engraveName = engrave.sliced(engraveNameStartIndex, engraveNameEndIndex - engraveNameStartIndex);
            int engraveValue = engrave.sliced(engraveValueStartIndex, engraveValueEndIndex - engraveValueStartIndex).toInt();

            if (ItemType::Accessory == pItem->itemType())
            {
                if (engraveName.contains("감소"))
                    static_cast<Accessory*>(pItem)->addPenalty(engraveName, engraveValue);
                else
                    static_cast<Accessory*>(pItem)->addEngrave(engraveName, engraveValue);
            }
            else if (ItemType::AbilityStone == pItem->itemType())
            {
                if (engraveName.contains("감소"))
                    static_cast<AbilityStone*>(pItem)->addPenalty(engraveName, engraveValue);
                else
                    static_cast<AbilityStone*>(pItem)->addEngrave(engraveName, engraveValue);
            }
        }
    }
}

Weapon *ResponseParser::parseWeapon(const QJsonObject &object)
{
    Weapon *pWeapon = new Weapon();

    parseItemInfo(object, pWeapon);

    const QJsonObject &tooltip = QJsonDocument::fromJson(object.find("Tooltip")->toString().toUtf8()).object();
    const QStringList &tooltipKeys = tooltip.keys();

    for (const QString &key : tooltipKeys)
    {
        const QJsonObject &element = tooltip.find(key)->toObject();
        const QString &type = element.find("type")->toString();
        const QVariant &value = element.find("value")->toVariant();

        if (type == "ItemTitle")
            parseItemTitle(value.toJsonObject(), pWeapon);
        else if (type == "ItemPartBox")
            parseItemPartBox(value.toJsonObject(), pWeapon);
        else if (type == "SingleTextBox")
            parseSingleTextBox(value.toString(), pWeapon);
    }

    return pWeapon;
}

Armor *ResponseParser::parseArmor(const QJsonObject &object)
{
    Armor *pArmor = new Armor();

    parseItemInfo(object, pArmor);

    const QJsonObject &tooltip = QJsonDocument::fromJson(object.find("Tooltip")->toString().toUtf8()).object();
    const QStringList &tooltipKeys = tooltip.keys();

    for (const QString &key : tooltipKeys)
    {
        const QJsonObject &element = tooltip.find(key)->toObject();
        const QString &type = element.find("type")->toString();
        const QVariant &value = element.find("value")->toVariant();

        if (value.isNull())
            continue;

        if (type == "ItemTitle")
            parseItemTitle(value.toJsonObject(), pArmor);
        else if (type == "ItemPartBox")
            parseItemPartBox(value.toJsonObject(), pArmor);
        else if (type == "IndentStringGroup")
            parseIndentStringGroup(value.toJsonObject(), pArmor);
    }

    return pArmor;
}

Accessory *ResponseParser::parseAccessory(const QJsonObject &object)
{
    Accessory *pAccessory = new Accessory();

    parseItemInfo(object, pAccessory);

    const QJsonObject &tooltip = QJsonDocument::fromJson(object.find("Tooltip")->toString().toUtf8()).object();
    const QStringList &tooltipKeys = tooltip.keys();

    for (const QString &key : tooltipKeys)
    {
        const QJsonObject &element = tooltip.find(key)->toObject();
        const QString &type = element.find("type")->toString();
        const QVariant &value = element.find("value")->toVariant();

        if (type == "ItemTitle")
            parseItemTitle(value.toJsonObject(), pAccessory);
        else if (type == "ItemPartBox")
            parseItemPartBox(value.toJsonObject(), pAccessory);
        else if (type == "IndentStringGroup")
            parseIndentStringGroup(value.toJsonObject(), pAccessory);
    }

    return pAccessory;
}

AbilityStone *ResponseParser::parseAbilityStone(const QJsonObject &object)
{
    AbilityStone *pAbilityStone = new AbilityStone();

    parseItemInfo(object, pAbilityStone);

    const QJsonObject &tooltip = QJsonDocument::fromJson(object.find("Tooltip")->toString().toUtf8()).object();
    const QStringList &tooltipKeys = tooltip.keys();

    for (const QString &key : tooltipKeys)
    {
        const QJsonObject &element = tooltip.find(key)->toObject();
        const QString &type = element.find("type")->toString();
        const QVariant &value = element.find("value")->toVariant();

        if (type == "IndentStringGroup")
            parseIndentStringGroup(value.toJsonObject(), pAbilityStone);
    }

    return pAbilityStone;
}

Bracelet *ResponseParser::parseBracelet(const QJsonObject &object)
{
    Bracelet *pBracelet = new Bracelet();

    parseItemInfo(object, pBracelet);

    const QJsonObject &tooltip = QJsonDocument::fromJson(object.find("Tooltip")->toString().toUtf8()).object();
    const QStringList &tooltipKeys = tooltip.keys();

    for (const QString &key : tooltipKeys)
    {
        const QJsonObject &element = tooltip.find(key)->toObject();
        const QString &type = element.find("type")->toString();
        const QVariant &value = element.find("value")->toVariant();

        if (type == "ItemPartBox")
            parseItemPartBox(value.toJsonObject(), pBracelet);
    }

    return pBracelet;
}
