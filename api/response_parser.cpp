#include "response_parser.h"
#include "game/skill/skill_manager.h"
#include "game/item/rune.h"
#include "function/character_search/character_search.h"

#include <QJsonObject>
#include <QJsonArray>

void ResponseParser::parseSibling(QJsonDocument response, Character *pCharacter, uint8_t statusBit)
{
    const QJsonArray &siblings = response.array();

    for (int i = 0; i < siblings.size(); i++)
    {
        const QJsonObject &sibling = siblings[i].toObject();

        Profile *pProfile = new Profile();
        pProfile->setServerName(sibling.find("ServerName")->toString());
        pProfile->setCharacterName(sibling.find("CharacterName")->toString());
        pProfile->setCharacterLevel(sibling.find("CharacterLevel")->toInt());
        pProfile->setCharacterClass(sibling.find("CharacterClassName")->toString());
        pProfile->setItemLevel(sibling.find("ItemAvgLevel")->toString().remove(",").toDouble());

        pCharacter->addSibling(pProfile);
    }

    // TODO. 다른 객체에서도 사용할 수 있도록 수정필요
    CharacterSearch::getInstance()->updateParseStatus(statusBit, pCharacter);
}

void ResponseParser::parseProfile(QJsonDocument response, Character *pCharacter, uint8_t statusBit)
{
    const QJsonObject &profile = response.object();

    Profile *pProfile = new Profile();
    pProfile->setExpeditionLevel(profile.find("ExpeditionLevel")->toInt());
    pProfile->setTitle(profile.find("Title")->toString());
    pProfile->setGuildName(profile.find("GuildName")->toString());
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
        else if (type != "공격력")
            pProfile->addAbility(qStringToAbility(type), value);
    }

    pCharacter->setProfile(pProfile);

    // TODO. 다른 객체에서도 사용할 수 있도록 수정필요
    CharacterSearch::getInstance()->updateParseStatus(statusBit, pCharacter);
}

void ResponseParser::parseEquipment(QJsonDocument response, Character *pCharacter, uint8_t statusBit)
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

    // TODO. 다른 객체에서도 사용할 수 있도록 수정필요
    CharacterSearch::getInstance()->updateParseStatus(statusBit, pCharacter);
}

void ResponseParser::parseSkill(QJsonDocument response, Character *pCharacter, uint8_t statusBit)
{
    // TODO. 무한 대기 개선 필요
    // Profile이 업데이트 될 때까지 대기
    while (pCharacter->getProfile() == nullptr);

    // 해당 직업의 스킬 목록 생성
    const QString &characterClass = pCharacter->getProfile()->characterClass();
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

    // TODO. 다른 객체에서도 사용할 수 있도록 수정필요
    CharacterSearch::getInstance()->updateParseStatus(statusBit, pCharacter);
}

void ResponseParser::parseEngrave(QJsonDocument response, Character *pCharacter, uint8_t statusBit)
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

    // TODO. 다른 객체에서도 사용할 수 있도록 수정필요
    CharacterSearch::getInstance()->updateParseStatus(statusBit, pCharacter);
}

void ResponseParser::parseCard(QJsonDocument response, Character *pCharacter, uint8_t statusBit)
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

    // TODO. 다른 객체에서도 사용할 수 있도록 수정필요
    CharacterSearch::getInstance()->updateParseStatus(statusBit, pCharacter);
}

void ResponseParser::parseGem(QJsonDocument response, Character *pCharacter, uint8_t statusBit)
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

        pCharacter->addGem(pGem);
    }

    // TODO. 다른 객체에서도 사용할 수 있도록 수정필요
    CharacterSearch::getInstance()->updateParseStatus(statusBit, pCharacter);
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

Weapon *ResponseParser::parseWeapon(const QJsonObject &object)
{
    Weapon *pWeapon = new Weapon();

    pWeapon->setItemName(object.find("Name")->toString());
    pWeapon->setIconPath(object.find("Icon")->toString());
    pWeapon->setItemGrade(qStringToItemGrade(object.find("Grade")->toString()));

    const QJsonObject &tooltip = QJsonDocument::fromJson(object.find("Tooltip")->toString().toUtf8()).object();
    const QStringList &tooltipKeys = tooltip.keys();

    for (const QString &key : tooltipKeys)
    {
        const QJsonObject &element = tooltip.find(key)->toObject();
        const QString &type = element.find("type")->toString();
        const QVariant &value = element.find("value")->toVariant();

        if (type == "ItemTitle")
        {
            const QJsonObject &itemTitle = value.toJsonObject();

            pWeapon->setQuality(itemTitle.find("qualityValue")->toInt());

            // 아이템 레벨, 티어 parsing
            const QString &leftStr2 = itemTitle.find("leftStr2")->toString();
            int itemLevelIndex = leftStr2.indexOf("아이템 레벨") + 7;
            int itemTierIndex = leftStr2.indexOf("티어");

            pWeapon->setItemLevel(leftStr2.sliced(itemLevelIndex, itemTierIndex - 2 - itemLevelIndex).toInt());
            pWeapon->setTier(leftStr2.sliced(itemTierIndex + 3, 1).toInt());
        }
        else if (type == "ItemPartBox")
        {
            const QJsonObject &itemPartBox = value.toJsonObject();

            // 세트효과 parsing
            if (itemPartBox.find("Element_000")->toString().contains("세트 효과 레벨"))
            {
                const QString &itemSetLevel = itemPartBox.find("Element_001")->toString();
                int setLevelIndex = itemSetLevel.indexOf("Lv") + 3;

                pWeapon->setItemSet(qStringToItemSet(itemSetLevel.sliced(0, 2)));
                pWeapon->setSetLevel(itemSetLevel.sliced(setLevelIndex, 1).toInt());
            }
        }
        else if (type == "SingleTextBox")
        {
            const QString &singleTextBox = value.toString();

            if (singleTextBox.contains("엘라 부여 완료"))
                pWeapon->setElla(true);
        }
    }

    // 강화 수치(reforge) parsing
    const QString &itemName = pWeapon->itemName();
    if (itemName.startsWith("+"))
        pWeapon->setReforge(itemName.sliced(1, 2).toInt());
    else
        pWeapon->setReforge(0);

    return pWeapon;
}

Armor *ResponseParser::parseArmor(const QJsonObject &object)
{
    const QStringList parts = {"투구", "어깨", "상의", "하의", "장갑", "공용"};

    Armor *pArmor = new Armor();

    pArmor->setArmorPart(static_cast<ArmorPart>(parts.indexOf(object.find("Type")->toString())));
    pArmor->setItemName(object.find("Name")->toString());
    pArmor->setIconPath(object.find("Icon")->toString());
    pArmor->setItemGrade(qStringToItemGrade(object.find("Grade")->toString()));

    const QJsonObject &tooltip = QJsonDocument::fromJson(object.find("Tooltip")->toString().toUtf8()).object();
    const QStringList &tooltipKeys = tooltip.keys();

    for (const QString &key : tooltipKeys)
    {
        const QJsonObject &element = tooltip.find(key)->toObject();
        const QString &type = element.find("type")->toString();
        const QVariant &value = element.find("value")->toVariant();

        if (type == "ItemTitle")
        {
            const QJsonObject &itemTitle = value.toJsonObject();

            pArmor->setQuality(itemTitle.find("qualityValue")->toInt());

            // 아이템 레벨, 티어 parsing
            const QString &leftStr2 = itemTitle.find("leftStr2")->toString();
            int itemLevelIndex = leftStr2.indexOf("아이템 레벨") + 7;
            int itemTierIndex = leftStr2.indexOf("티어");

            pArmor->setItemLevel(leftStr2.sliced(itemLevelIndex, itemTierIndex - 2 - itemLevelIndex).toInt());
            pArmor->setTier(leftStr2.sliced(itemTierIndex + 3, 1).toInt());
        }
        else if (type == "ItemPartBox")
        {
            const QJsonObject &itemPartBox = value.toJsonObject();

            // 세트효과 parsing
            if (itemPartBox.find("Element_000")->toString().contains("세트 효과 레벨"))
            {
                const QString &itemSetLevel = itemPartBox.find("Element_001")->toString();
                int setLevelIndex = itemSetLevel.indexOf("Lv") + 3;

                pArmor->setItemSet(qStringToItemSet(itemSetLevel.sliced(0, 2)));
                pArmor->setSetLevel(itemSetLevel.sliced(setLevelIndex, 1).toInt());
            }
        }
        else if (type == "IndentStringGroup")
        {
            const QJsonObject &element = value.toJsonObject().find("Element_000")->toObject();

            // 엘릭서 parsing
            if (element.find("topStr")->toString().contains("엘릭서"))
            {
                const QJsonObject &elixirs = element.find("contentStr")->toObject();
                const QStringList &keys = elixirs.keys();

                for (const QString &key : keys)
                {
                    const QString &elixir = elixirs.find(key)->toObject()
                                                   .find("contentStr")->toString();
                    int partIndex = elixir.indexOf("[") + 1;
                    int levelIndex = elixir.indexOf("Lv") + 3;

                    QString part = elixir.sliced(partIndex, 2);
                    QString effect = elixir.sliced(partIndex + 11, levelIndex - 26 - (partIndex + 11));
                    int level = elixir.sliced(levelIndex, 1).toInt();

                    pArmor->addElixir({static_cast<ArmorPart>(parts.indexOf(part)), effect, level});
                }
            }
        }
    }

    // 강화 수치(reforge) parsing
    const QString &itemName = pArmor->itemName();
    if (itemName.startsWith("+"))
        pArmor->setReforge(itemName.sliced(1, 2).toInt());
    else
        pArmor->setReforge(0);

    return pArmor;
}

Accessory *ResponseParser::parseAccessory(const QJsonObject &object)
{
    Accessory *pAccessory = new Accessory();

    const QString &type = object.find("Type")->toString();

    if (type == "목걸이") pAccessory->setAccessoryPart(AccessoryPart::Necklace);
    else if (type == "귀걸이") pAccessory->setAccessoryPart(AccessoryPart::Earring);
    else if (type == "반지") pAccessory->setAccessoryPart(AccessoryPart::Ring);

    pAccessory->setItemName(object.find("Name")->toString());
    pAccessory->setIconPath(object.find("Icon")->toString());
    pAccessory->setItemGrade(qStringToItemGrade(object.find("Grade")->toString()));

    const QJsonObject &tooltip = QJsonDocument::fromJson(object.find("Tooltip")->toString().toUtf8()).object();
    const QStringList &tooltipKeys = tooltip.keys();

    for (const QString &key : tooltipKeys)
    {
        const QJsonObject &element = tooltip.find(key)->toObject();
        const QString &type = element.find("type")->toString();
        const QVariant &value = element.find("value")->toVariant();

        // 품질 parsing
        if (type == "ItemTitle")
            pAccessory->setQuality(value.toJsonObject().find("qualityValue")->toInt());
        else if (type == "ItemPartBox")
        {
            const QJsonObject &itemPartBox = value.toJsonObject();

            // 특성 parsing
            if (itemPartBox.find("Element_000")->toString().contains("추가 효과"))
            {
                const QString &abilities = itemPartBox.find("Element_001")->toString();
                int abilityIndex = 0;
                int valueIndex = abilities.indexOf("+");

                while (valueIndex != -1)
                {
                    const QString &abilityStr = abilities.sliced(abilityIndex, 2);
                    int abilityValue = abilities.sliced(valueIndex + 1, 3).toInt();

                    pAccessory->setAbility(qStringToAbility(abilityStr), abilityValue);

                    abilityIndex = abilities.indexOf(">") + 1;
                    valueIndex = abilities.indexOf("+", valueIndex + 1);
                }
            }
        }
        else if (type == "IndentStringGroup")
        {
            // 각인 parsing
            const QJsonObject &engraves = value.toJsonObject().find("Element_000")->toObject()
                                                              .find("contentStr")->toObject();

            for (int i = 0; i < 3; i++)
            {
                const QString &engrave = engraves.find(QString("Element_00%1").arg(i))->toObject()
                                                 .find("contentStr")->toString();
                int engraveNameIndex = engrave.indexOf(">") + 1;
                int engraveValueIndex = engrave.indexOf("+") + 1;

                const QString &engraveName = engrave.sliced(engraveNameIndex, engrave.indexOf("</FONT>") - engraveNameIndex);
                int engraveValue = engrave.sliced(engraveValueIndex, 1).toInt();

                if (i != 2)
                    pAccessory->addEngrave(engraveName, engraveValue);
                else
                    pAccessory->addPenalty(engraveName, engraveValue);
            }
        }
    }

    return pAccessory;
}

AbilityStone *ResponseParser::parseAbilityStone(const QJsonObject &object)
{
    AbilityStone *pAbilityStone = new AbilityStone();

    pAbilityStone->setItemName(object.find("Name")->toString());
    pAbilityStone->setIconPath(object.find("Icon")->toString());
    pAbilityStone->setItemGrade(qStringToItemGrade(object.find("Grade")->toString()));

    const QJsonObject &tooltip = QJsonDocument::fromJson(object.find("Tooltip")->toString().toUtf8()).object();
    const QStringList &tooltipKeys = tooltip.keys();

    for (const QString &key : tooltipKeys)
    {
        const QJsonObject &element = tooltip.find(key)->toObject();
        const QString &type = element.find("type")->toString();
        const QVariant &value = element.find("value")->toVariant();

        if (type == "IndentStringGroup")
        {
            // 각인 parsing
            const QJsonObject &engraves = value.toJsonObject().find("Element_000")->toObject()
                                                              .find("contentStr")->toObject();

            for (int i = 0; i < 3; i++)
            {
                const QString &engrave = engraves.find(QString("Element_00%1").arg(i))->toObject()
                                                 .find("contentStr")->toString();
                int engraveNameIndex = engrave.indexOf(">") + 1;
                int engraveValueIndex = engrave.indexOf("+") + 1;

                const QString &engraveName = engrave.sliced(engraveNameIndex, engrave.indexOf("</FONT>") - engraveNameIndex);
                int engraveValue = engrave.sliced(engraveValueIndex, 1).toInt();

                if (i != 2)
                    pAbilityStone->addEngrave(engraveName, engraveValue);
                else
                    pAbilityStone->addPenalty(engraveName, engraveValue);
            }
        }
    }

    return pAbilityStone;
}

Bracelet *ResponseParser::parseBracelet(const QJsonObject &object)
{
    Bracelet *pBracelet = new Bracelet();

    pBracelet->setItemName(object.find("Name")->toString());
    pBracelet->setIconPath(object.find("Icon")->toString());
    pBracelet->setItemGrade(qStringToItemGrade(object.find("Grade")->toString()));

    const QJsonObject &tooltip = QJsonDocument::fromJson(object.find("Tooltip")->toString().toUtf8()).object();
    const QStringList &tooltipKeys = tooltip.keys();

    for (const QString &key : tooltipKeys)
    {
        const QJsonObject &element = tooltip.find(key)->toObject();
        const QString &type = element.find("type")->toString();
        const QVariant &value = element.find("value")->toVariant();

        // 팔찌 효과 parsing
        if (type == "ItemPartBox")
        {
            const QString &effectStr = value.toJsonObject().find("Element_001")->toString();

            // 팔찌 효과는 </img> 로 시작
            int startIndex = effectStr.indexOf("</img>");
            // </img>이후 처음 등장하는 <BR> 사이에 팔찌 효과가 존재
            int endIndex = effectStr.indexOf("<BR>", startIndex);

            while (startIndex != -1)
            {
                // <BR>이 더 이상 존재하지 않으면 마지막 효과
                if (endIndex == -1)
                    endIndex = effectStr.size();
                startIndex += QString("</img>").size();

                QString effectStrParsed = effectStr.sliced(startIndex, endIndex - startIndex);

                // 공백 제거
                if (effectStrParsed.front() == ' ')
                    effectStrParsed = effectStrParsed.sliced(1, effectStrParsed.size() - 1);

                // [ 로 시작하면 특수 효과, 아니면 일반 효과
                if (effectStrParsed.front() == '[')
                {
                    // 특수 효과명은 <FONT> 태그로 묶여있음
                    int effectStartIndex = effectStrParsed.indexOf(">") + 1;
                    int effectEndIndex = effectStrParsed.indexOf("</FONT>");

                    // TODO. 특수 효과 value parsing
                    pBracelet->addSpecialEffect(effectStrParsed.sliced(effectStartIndex, effectEndIndex - effectStartIndex), 0);
                }
                else
                {
                    // 효과명과 값 parsing 후 추가
                    int valueStartIndex = effectStrParsed.indexOf("+") + 1;
                    const QString &effect = effectStrParsed.sliced(0, valueStartIndex - 2);
                    int value = effectStrParsed.sliced(valueStartIndex, effectStrParsed.size() - valueStartIndex).toInt();

                    pBracelet->addEffect(effect, value);
                }

                // 다음 효과 탐색
                startIndex = effectStr.indexOf("</img>", startIndex);
                endIndex = effectStr.indexOf("<BR>", startIndex);
            }
        }
    }

    return pBracelet;
}
