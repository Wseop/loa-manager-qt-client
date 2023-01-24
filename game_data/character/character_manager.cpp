#include "character_manager.h"
#include "api/enum/lostark_api.h"
#include "api/api_manager.h"
#include "game_data/character/character.h"
#include "game_data/character/profile/enum/class.h"
#include "game_data/character/profile/profile.h"
#include "game_data/character/item/equip.h"
#include "game_data/character/item/accessory.h"
#include "game_data/character/item/abilitystone.h"
#include "game_data/character/item/bracelet.h"
#include "game_data/character/item/gem.h"
#include "game_data/character/skill/skill.h"
#include "game_data/character/skill/skillrune.h"
#include "game_data/character/card/card.h"
#include "game_data/character/collectible/collectible.h"
#include "function/character_search/character_search.h"
#include "db/db_manager.h"
#include "db/document_builder.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QThread>

static void tDbInsertCharacter(QString name, QString server, Class cls, double level)
{
    bsoncxx::builder::stream::document doc = DocumentBuilder::buildDocumentCharacter(name, server, cls, level);

    bsoncxx::builder::stream::document filter{};
    filter << "Name" << name.toStdString();

    DbManager* pDbManager = DbManager::getInstance();
    pDbManager->lock();
    DbManager::getInstance()->insertDocument(Collection::Character, doc.extract(), filter.extract());
    pDbManager->unlock();
}

CharacterManager* CharacterManager::m_pInstance = nullptr;

CharacterManager::CharacterManager() :
    m_handlerStatus(0),
    m_regExpHtmlTag("<[^>]*>")
{
    initNetworkManagers();
}

CharacterManager::~CharacterManager()
{
    destroyInstance();
}

void CharacterManager::initNetworkManagers()
{
    // create networkManagers
    for (int i = 0; i < CHARACTER_API_COUNT; i++)
        m_networkManagers.append(new QNetworkAccessManager());

    // connect reply handlers
    connect(m_networkManagers[static_cast<int>(LostarkApi::Sibling)], &QNetworkAccessManager::finished, this, &CharacterManager::handleSibling);
    connect(m_networkManagers[static_cast<int>(LostarkApi::Profile)], &QNetworkAccessManager::finished, this, &CharacterManager::handleProfile);
    connect(m_networkManagers[static_cast<int>(LostarkApi::Equipment)], &QNetworkAccessManager::finished, this, &CharacterManager::handleEquipment);
    connect(m_networkManagers[static_cast<int>(LostarkApi::Skill)], &QNetworkAccessManager::finished, this, &CharacterManager::handleSkill);
    connect(m_networkManagers[static_cast<int>(LostarkApi::Engrave)], &QNetworkAccessManager::finished, this, &CharacterManager::handleEngrave);
    connect(m_networkManagers[static_cast<int>(LostarkApi::Card)], &QNetworkAccessManager::finished, this, &CharacterManager::handleCard);
    connect(m_networkManagers[static_cast<int>(LostarkApi::Gem)], &QNetworkAccessManager::finished, this, &CharacterManager::handleGem);
    connect(m_networkManagers[static_cast<int>(LostarkApi::Collectible)], &QNetworkAccessManager::finished, this, &CharacterManager::handleCollectible);
}

void CharacterManager::requestCharacterInfo()
{
    ApiManager* pApiManager = ApiManager::getInstance();

    for (int i = 0; i < CHARACTER_API_COUNT; i++)
        pApiManager->get(m_networkManagers[i], static_cast<LostarkApi>(i), m_requestedCharacterName);
}

void CharacterManager::handleSibling(QNetworkReply* pReply)
{
    QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
    if (response.isNull())
    {
        setHandlerStatus(1 << static_cast<int>(LostarkApi::Sibling));
        return;
    }

    // handle Sibling data
    Character* pCharacter = m_nameToCharacter[m_requestedCharacterName];
    const QJsonArray& others = response.array();
    for (const QJsonValue& other : others)
    {
        const QJsonObject& otherObj = other.toObject();
        const QString& server = otherObj.find("ServerName")->toString();
        const QString& name = otherObj.find("CharacterName")->toString();
        const int& level = otherObj.find("CharacterLevel")->toInt();
        const Class& cls = strToClass(otherObj.find("CharacterClassName")->toString());
        const double& itemLevel = otherObj.find("ItemAvgLevel")->toString().remove(",").toDouble();

        pCharacter->addOther(Other{server, name, level, cls, itemLevel});
    }

    setHandlerStatus(1 << static_cast<int>(LostarkApi::Sibling));
}

void CharacterManager::handleProfile(QNetworkReply* pReply)
{
    QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
    if (response.isNull())
    {
        setHandlerStatus(1 << static_cast<int>(LostarkApi::Profile));
        return;
    }

    // handle Profile data
    Character* pCharacter = m_nameToCharacter[m_requestedCharacterName];
    const QJsonObject& profileObj = response.object();
    Profile* pProfile = new Profile();
    pProfile->setClass(strToClass(profileObj.find("CharacterClassName")->toString()));
    pProfile->setCharacterLevel(profileObj.find("CharacterLevel")->toInt());
    pProfile->setCharacterName(profileObj.find("CharacterName")->toString());
    pProfile->setServer(profileObj.find("ServerName")->toString());
    pProfile->setTitle(profileObj.find("Title")->toString());
    pProfile->setGuild(profileObj.find("GuildName")->toString());
    pProfile->setExpLevel(profileObj.find("ExpeditionLevel")->toInt());
    pProfile->setItemLevel(profileObj.find("ItemAvgLevel")->toString().remove(",").toDouble());

    const QJsonArray& abilities = profileObj.find("Stats")->toArray();
    for (const QJsonValue& value : abilities)
    {
        const QJsonObject& abilityObj = value.toObject();
        Ability ability = strToAbility(abilityObj.find("Type")->toString());
        if (ability != Ability::Size)
        {
            int abilityValue = abilityObj.find("Value")->toString().toInt();
            pProfile->addAbility(ability, abilityValue);
        }
    }
    pCharacter->setProfile(pProfile);

    setHandlerStatus(1 << static_cast<int>(LostarkApi::Profile));
}

void CharacterManager::handleEquipment(QNetworkReply* pReply)
{
    QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
    if (response.isNull())
    {
        setHandlerStatus(1 << static_cast<int>(LostarkApi::Equipment));
        return;
    }

    // handle Equipment data
    Character* pCharacter = m_nameToCharacter[m_requestedCharacterName];
    const QJsonArray& equipments = response.array();
    for (const QJsonValue& value : equipments)
    {
        const QJsonObject& equipObj = value.toObject();
        ItemType itemType = strToItemType(equipObj.find("Type")->toString());
        if (itemType == ItemType::무기)
        {
            Equip* pEquip = new Equip(itemType);
            pEquip->setName(equipObj.find("Name")->toString());
            pEquip->setIconPath(equipObj.find("Icon")->toString());
            pEquip->setGrade(strToItemGrade(equipObj.find("Grade")->toString()));

            // remove escape sequences by casting twice (to string -> to object)
            const QJsonObject& tooltipObj = QJsonDocument::fromJson(equipObj.find("Tooltip")->toString().toUtf8()).object();
            const QJsonObject& itemTitleObj = tooltipObj.find("Element_001")->toObject().find("value")->toObject();
            pEquip->setLevelTier(itemTitleObj.find("leftStr2")->toString().remove(m_regExpHtmlTag));
            pEquip->setQuality(itemTitleObj.find("qualityValue")->toInt());

            const QStringList& elements = tooltipObj.keys();
            for (const QString& element : elements)
            {
                const QJsonObject& elementObj = tooltipObj.find(element)->toObject();
                const QString& type = elementObj.find("type")->toString();

                if (type == "ItemPartBox")
                {
                    const QJsonObject& valueObj = elementObj.find("value")->toObject();
                    if (valueObj.find("Element_000")->toString().contains("세트 효과 레벨"))
                    {
                        const QString& setEffectLevel = valueObj.find("Element_001")->toString();
                        pEquip->setSetEffect(strToSetEffect(setEffectLevel.sliced(0, 2)));
                        pEquip->setSetLevel(setEffectLevel.sliced(setEffectLevel.indexOf("Lv.") + 3, 1).toInt());
                    }
                }
                else if (type == "SingleTextBox")
                {
                    const QString& valueStr = elementObj.find("value")->toString();
                    if (valueStr.contains("엘라 부여 완료"))
                        pEquip->setElla(true);
                }
            }
            if (pEquip->getGrade() == ItemGrade::에스더)
                pEquip->setSetEffect(SetEffect::에스더);

            pCharacter->setEquip(itemType, pEquip);
        }
        else if (itemType >= ItemType::투구 && itemType <= ItemType::어깨)
        {
            Equip* pEquip = new Equip(itemType);
            pEquip->setName(equipObj.find("Name")->toString());
            pEquip->setIconPath(equipObj.find("Icon")->toString());
            pEquip->setGrade(strToItemGrade(equipObj.find("Grade")->toString()));

            const QJsonObject& tooltipObj = QJsonDocument::fromJson(equipObj.find("Tooltip")->toString().toUtf8()).object();
            const QJsonObject& itemTitleObj = tooltipObj.find("Element_001")->toObject().find("value")->toObject();
            pEquip->setLevelTier(itemTitleObj.find("leftStr2")->toString().remove(m_regExpHtmlTag));
            pEquip->setQuality(itemTitleObj.find("qualityValue")->toInt());

            const QStringList& elements = tooltipObj.keys();
            for (const QString& element : elements)
            {
                const QJsonObject& elementObj = tooltipObj.find(element)->toObject();
                const QString& type = elementObj.find("type")->toString();
                if (type == "ItemPartBox")
                {
                    const QJsonObject& valueObj = elementObj.find("value")->toObject();
                    if (valueObj.find("Element_000")->toString().contains("세트 효과 레벨"))
                    {
                        const QString& setEffectLevel = valueObj.find("Element_001")->toString();
                        pEquip->setSetEffect(strToSetEffect(setEffectLevel.sliced(0, 2)));
                        pEquip->setSetLevel(setEffectLevel.sliced(setEffectLevel.indexOf("Lv.") + 3, 1).toInt());
                    }
                }
            }

            pCharacter->setEquip(itemType, pEquip);
        }
        else if (itemType == ItemType::목걸이)
        {
            Accessory* pAccessory = new Accessory(itemType);
            pAccessory->setName(equipObj.find("Name")->toString());
            pAccessory->setIconPath(equipObj.find("Icon")->toString());
            pAccessory->setGrade(strToItemGrade(equipObj.find("Grade")->toString()));

            const QJsonObject& tooltipObj = QJsonDocument::fromJson(equipObj.find("Tooltip")->toString().toUtf8()).object();
            pAccessory->setQuality(tooltipObj.find("Element_001")->toObject().find("value")->toObject().find("qualityValue")->toInt());

            const QStringList& elements = tooltipObj.keys();
            for (const QString& element : elements)
            {
                const QJsonObject& elementObj = tooltipObj.find(element)->toObject();
                const QString& type = elementObj.find("type")->toString();
                if (type == "ItemPartBox")
                {
                    // 특성
                    const QJsonObject& valueObj = elementObj.find("value")->toObject();
                    if (valueObj.find("Element_000")->toString().contains("추가 효과"))
                    {
                        const QString& abilitiesStr = valueObj.find("Element_001")->toString();
                        int startIdx, endIdx;
                        QString abilityStr;
                        int abilityValue = 0;

                        // 특성1
                        startIdx = 0;
                        endIdx = abilitiesStr.indexOf("<BR>");
                        abilityStr = abilitiesStr.sliced(startIdx, 2);
                        startIdx = abilitiesStr.indexOf("+") + 1;
                        abilityValue = abilitiesStr.sliced(startIdx, endIdx - startIdx).toInt();
                        pAccessory->addAbility(strToAbility(abilityStr), abilityValue);
                        // 특성2
                        startIdx = endIdx + 4;
                        endIdx = abilitiesStr.size();
                        abilityStr = abilitiesStr.sliced(startIdx, 2);
                        startIdx = abilitiesStr.indexOf("+", startIdx) + 1;
                        abilityValue = abilitiesStr.sliced(startIdx, endIdx - startIdx).toInt();
                        pAccessory->addAbility(strToAbility(abilityStr), abilityValue);
                    }
                }
                else if (type == "IndentStringGroup")
                {
                    // 각인
                    const QJsonObject& engravesObj = elementObj.find("value")->toObject()
                                                              .find("Element_000")->toObject()
                                                              .find("contentStr")->toObject();
                    const QStringList& keys = engravesObj.keys();
                    for (const QString& key : keys)
                    {
                        const QString& engraveStr = engravesObj.find(key)->toObject().find("contentStr")->toString();
                        // 각인명
                        int startIdx = engraveStr.indexOf(">") + 1;
                        int endIdx = engraveStr.indexOf("</FONT>");
                        QString engrave = engraveStr.sliced(startIdx, endIdx - startIdx);
                        // 각인값
                        int valueIdx = engraveStr.indexOf("+") + 1;
                        int engraveValue = engraveStr.sliced(valueIdx, 1).toInt();

                        if (!engrave.contains("감소"))
                            pAccessory->addEngrave(engrave, engraveValue);
                        else
                            pAccessory->addPenalty(engrave, engraveValue);
                    }
                }
            }

            pCharacter->setAccessory(itemType, pAccessory);
        }
        else if (itemType == ItemType::귀걸이 || itemType == ItemType::반지)
        {
            Accessory* pAccessory = new Accessory(itemType);
            pAccessory->setName(equipObj.find("Name")->toString());
            pAccessory->setIconPath(equipObj.find("Icon")->toString());
            pAccessory->setGrade(strToItemGrade(equipObj.find("Grade")->toString()));

            const QJsonObject& tooltipObj = QJsonDocument::fromJson(equipObj.find("Tooltip")->toString().toUtf8()).object();
            pAccessory->setQuality(tooltipObj.find("Element_001")->toObject().find("value")->toObject().find("qualityValue")->toInt());

            const QStringList& elements = tooltipObj.keys();
            for (const QString& element : elements)
            {
                const QJsonObject& elementObj = tooltipObj.find(element)->toObject();
                const QString& type = elementObj.find("type")->toString();
                if (type == "ItemPartBox")
                {
                    // 특성
                    const QJsonObject& valueObj = elementObj.find("value")->toObject();
                    if (valueObj.find("Element_000")->toString().contains("추가 효과"))
                    {
                        const QString& abilityStr = valueObj.find("Element_001")->toString();
                        QString ability = abilityStr.sliced(0, 2);
                        int index = abilityStr.indexOf("+") + 1;
                        int abilityValue = abilityStr.sliced(index, abilityStr.size() - index).toInt();
                        pAccessory->addAbility(strToAbility(ability), abilityValue);
                    }
                }
                else if (type == "IndentStringGroup")
                {
                    // 각인
                    const QJsonObject& engravesObj = elementObj.find("value")->toObject()
                                                              .find("Element_000")->toObject()
                                                              .find("contentStr")->toObject();
                    const QStringList& keys = engravesObj.keys();
                    for (const QString& key : keys)
                    {
                        const QString& engraveStr = engravesObj.find(key)->toObject().find("contentStr")->toString();
                        // 각인명
                        int startIdx = engraveStr.indexOf(">") + 1;
                        int endIdx = engraveStr.indexOf("</FONT>");
                        QString engrave = engraveStr.sliced(startIdx, endIdx - startIdx);
                        // 각인값
                        int valueIdx = engraveStr.indexOf("+") + 1;
                        int engraveValue = engraveStr.sliced(valueIdx, 1).toInt();

                        if (!engrave.contains("감소"))
                            pAccessory->addEngrave(engrave, engraveValue);
                        else
                            pAccessory->addPenalty(engrave, engraveValue);
                    }
                }
            }

            pCharacter->setAccessory(itemType, pAccessory);
        }
        else if (itemType == ItemType::어빌리티_스톤)
        {
            AbilityStone* pStone = new AbilityStone();
            pStone->setName(equipObj.find("Name")->toString());
            pStone->setIconPath(equipObj.find("Icon")->toString());
            pStone->setGrade(strToItemGrade(equipObj.find("Grade")->toString()));

            const QJsonObject& tooltipObj = QJsonDocument::fromJson(equipObj.find("Tooltip")->toString().toUtf8()).object();
            const QStringList& elements = tooltipObj.keys();
            for (const QString& element : elements)
            {
                const QJsonObject& elementObj = tooltipObj.find(element)->toObject();
                const QString& type = elementObj.find("type")->toString();
                if (type == "IndentStringGroup")
                {
                    const QJsonObject& engravesObj = elementObj.find("value")->toObject()
                                                               .find("Element_000")->toObject()
                                                               .find("contentStr")->toObject();
                    for (int i = 0; i < 2; i++)
                    {
                        QString key = QString("Element_00%1").arg(i);
                        const QString& engraveStr = engravesObj.find(key)->toObject().find("contentStr")->toString();

                        int startIdx = engraveStr.indexOf(">") + 1;
                        int endIdx = engraveStr.indexOf("</FONT>");
                        QString engrave = engraveStr.sliced(startIdx, endIdx - startIdx);

                        startIdx = engraveStr.indexOf("+") + 1;
                        endIdx = engraveStr.indexOf("<BR>");
                        int engraveValue = engraveStr.sliced(startIdx, endIdx - startIdx).toInt();

                        pStone->addEngrave(engrave, engraveValue);
                    }

                    const QString& penaltyStr = engravesObj.find("Element_002")->toObject().find("contentStr")->toString();

                    int startIdx = penaltyStr.indexOf(">") + 1;
                    int endIdx = penaltyStr.indexOf("</FONT>");
                    QString penalty = penaltyStr.sliced(startIdx, endIdx - startIdx);

                    startIdx = penaltyStr.indexOf("+") + 1;
                    endIdx = penaltyStr.indexOf("<BR>");
                    int penaltyValue = penaltyStr.sliced(startIdx, endIdx - startIdx).toInt();

                    pStone->addPenalty(penalty, penaltyValue);
                }
            }

            pCharacter->setAbilityStone(pStone);
        }
        else if (itemType == ItemType::팔찌)
        {
            Bracelet* pBracelet = new Bracelet();
            pBracelet->setName(equipObj.find("Name")->toString());
            pBracelet->setIconPath(equipObj.find("Icon")->toString());
            pBracelet->setGrade(strToItemGrade(equipObj.find("Grade")->toString()));

            const QJsonObject& tooltipObj = QJsonDocument::fromJson(equipObj.find("Tooltip")->toString().toUtf8()).object();
            const QStringList& elements = tooltipObj.keys();
            for (const QString& element : elements)
            {
                const QJsonObject& elementObj = tooltipObj.find(element)->toObject();
                const QString& type = elementObj.find("type")->toString();
                if (type == "ItemPartBox")
                {
                    const QJsonObject& valueObj = elementObj.find("value")->toObject();
                    if (valueObj.find("Element_000")->toString().contains("팔찌 효과"))
                    {
                        QString effect = valueObj.find("Element_001")->toString();
                        effect.replace("</img>", "@");
                        effect.remove(m_regExpHtmlTag);

                        int startIdx = effect.indexOf("@");
                        int endIdx;
                        while (startIdx != -1)
                        {
                            startIdx++;
                            if (effect[startIdx] == ' ')
                                startIdx++;
                            endIdx = effect.indexOf("@", startIdx);
                            if (endIdx == -1)
                                endIdx = effect.size();

                            QString slicedStr = effect.sliced(startIdx, endIdx - startIdx);
                            if (slicedStr.startsWith('['))
                            {
                                slicedStr = slicedStr.sliced(0, slicedStr.indexOf("]") + 1);
                            }
                            pBracelet->addEffect(slicedStr);

                            startIdx = effect.indexOf("@", startIdx);
                        }
                    }
                }
            }

            pCharacter->setBracelet(pBracelet);
        }
    }

    setHandlerStatus(1 << static_cast<int>(LostarkApi::Equipment));
}

void CharacterManager::handleSkill(QNetworkReply* pReply)
{
    QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
    if (response.isNull())
    {
        setHandlerStatus(1 << static_cast<int>(LostarkApi::Skill));
        return;
    }

    // handle Skill data
    Character* pCharacter = m_nameToCharacter[m_requestedCharacterName];
    const QJsonArray& skills = response.array();
    for (const QJsonValue& value : skills)
    {
        const QJsonObject& skillObj = value.toObject();
        int level = skillObj.find("Level")->toInt();
        bool bRune = !skillObj.find("Rune")->isNull();
        if (level > 1 || bRune)
        {
            Skill* pSkill = new Skill();
            pSkill->setName(skillObj.find("Name")->toString());
            pSkill->setIconPath(skillObj.find("Icon")->toString());
            pSkill->setLevel(level);

            const QJsonArray& tripods = skillObj.find("Tripods")->toArray();
            for (const QJsonValue& tripodValue : tripods)
            {
                const QJsonObject& tripodObj = tripodValue.toObject();
                if (tripodObj.find("IsSelected")->toBool())
                {
                    Tripod tripod;
                    tripod.name = tripodObj.find("Name")->toString();
                    tripod.tier = tripodObj.find("Tier")->toInt();
                    tripod.level = tripodObj.find("Level")->toInt();
                    pSkill->addTripod(tripod);
                }
            }

            if (bRune)
            {
                const QJsonObject& runeObj = skillObj.find("Rune")->toObject();
                QString name = runeObj.find("Name")->toString();
                ItemGrade grade = strToItemGrade(runeObj.find("Grade")->toString());
                QString iconPath = runeObj.find("Icon")->toString();

                SkillRune* pRune = new SkillRune(name, grade, iconPath);
                pSkill->setRune(pRune);
            }

            pCharacter->addSkill(pSkill);
        }
    }

    setHandlerStatus(1 << static_cast<int>(LostarkApi::Skill));
}

void CharacterManager::handleEngrave(QNetworkReply* pReply)
{
    QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
    if (response.isNull())
    {
        setHandlerStatus(1 << static_cast<int>(LostarkApi::Engrave));
        return;
    }

    // handle Engrave data
    Character* pCharacter = m_nameToCharacter[m_requestedCharacterName];
    const QJsonArray& engraves = response.object().find("Effects")->toArray();
    Engrave* pEngrave = new Engrave();

    for (const QJsonValue& value : engraves)
    {
        const QJsonObject& engraveObj = value.toObject();
        QString nameLevel = engraveObj.find("Name")->toString();
        int endIndex = nameLevel.indexOf("Lv.") - 1;

        QString name = nameLevel.sliced(0, endIndex);
        int level = nameLevel.last(1).toInt();
        if (!name.contains("감소"))
            pEngrave->addEngrave(name, level * 5);
        else
            pEngrave->addPenalty(name, level * 5);
    }
    pCharacter->setEngrave(pEngrave);

    setHandlerStatus(1 << static_cast<int>(LostarkApi::Engrave));
}

void CharacterManager::handleCard(QNetworkReply* pReply)
{
    QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
    if (response.isNull())
    {
        setHandlerStatus(1 << static_cast<int>(LostarkApi::Card));
        return;
    }

    // handle Card data
    Character* pCharacter = m_nameToCharacter[m_requestedCharacterName];
    const QJsonArray& cards = response.object().find("Effects")->toArray();
    for (const QJsonValue& value : cards)
    {
        Card* pCard = new Card();

        const QJsonArray& cardEffects = value.toObject().find("Items")->toArray();
        for (const QJsonValue& effectValue : cardEffects)
        {
            const QJsonObject& effectObj = effectValue.toObject();
            QString name = effectObj.find("Name")->toString();
            QString desc = effectObj.find("Description")->toString();
            pCard->addCardEffect(name, desc);
        }

        if (pCard->getEffectNames().size() != 0)
            pCharacter->addCard(pCard);
        else
            delete pCard;
    }

    setHandlerStatus(1 << static_cast<int>(LostarkApi::Card));
}

void CharacterManager::handleGem(QNetworkReply* pReply)
{
    QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
    if (response.isNull())
    {
        setHandlerStatus(1 << static_cast<int>(LostarkApi::Gem));
        return;
    }

    // handle Gem data
    Character* pCharacter = m_nameToCharacter[m_requestedCharacterName];
    QMap<int, Gem*> slotToGem;
    const QJsonObject& gemData = response.object();
    const QJsonArray& gems = gemData.find("Gems")->toArray();
    const QJsonArray& gemEffects = gemData.find("Effects")->toArray();

    for (const QJsonValue& gemValue : gems)
    {
        const QJsonObject& gemObj = gemValue.toObject();
        int slot = gemObj.find("Slot")->toInt();

        Gem* pGem = new Gem();
        pGem->setName(gemObj.find("Name")->toString().remove(m_regExpHtmlTag));
        pGem->setIconPath(gemObj.find("Icon")->toString());
        pGem->setLevel(gemObj.find("Level")->toInt());
        pGem->setGrade(strToItemGrade(gemObj.find("Grade")->toString()));
        if (pGem->getName().contains("멸화"))
            pGem->setGemType(GemType::멸화);
        else if (pGem->getName().contains("홍염"))
            pGem->setGemType(GemType::홍염);
        else
            pGem->setGemType(GemType::Size);
        slotToGem[slot] = pGem;
    }

    QList<Gem*> gemList;
    for (const QJsonValue& effectValue : gemEffects)
    {
        const QJsonObject& effectObj = effectValue.toObject();
        int slot = effectObj.find("GemSlot")->toInt();
        QString effectStr = QString("%1").arg(effectObj.find("Name")->toString());

        Gem* pGem = slotToGem[slot];
        pGem->setEffect(effectStr);

        gemList.append(pGem);
    }

    std::sort(gemList.begin(), gemList.end(), [&](Gem* a, Gem* b){
        return a->getLevel() > b->getLevel();
    });

    for (Gem* pGem : gemList)
    {
        pCharacter->addGem(pGem);
    }

    setHandlerStatus(1 << static_cast<int>(LostarkApi::Gem));
}

void CharacterManager::handleCollectible(QNetworkReply* pReply)
{
    QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
    if (response.isNull())
    {
        setHandlerStatus(1 << static_cast<int>(LostarkApi::Collectible));
        return;
    }

    // handle Collectible data
    Character* pCharacter = m_nameToCharacter[m_requestedCharacterName];
    const QJsonArray& collectibles = response.array();
    for (const QJsonValue& data : collectibles)
    {
        const QJsonObject& collectible = data.toObject();
        Collectible* pCollectible = new Collectible();
        pCollectible->setType(strToCollectibleType(collectible.find("Type")->toString()));
        pCollectible->setMaxPoint(collectible.find("MaxPoint")->toInt());
        pCollectible->setPoint(collectible.find("Point")->toInt());
        pCharacter->addCollectible(pCollectible);
    }

    setHandlerStatus(1 << static_cast<int>(LostarkApi::Collectible));
}

void CharacterManager::setHandlerStatus(uint8_t bit)
{
    m_handlerStatus |= bit;

    if (m_handlerStatus == HANDLER_STATUS_FINISH)
    {
        Character* pCharacter = m_nameToCharacter[m_requestedCharacterName];
        const Profile* pProfile = pCharacter->getProfile();
        if (pProfile != nullptr)
        {
            // insert to db
            QThread* pThread = QThread::create(tDbInsertCharacter, pProfile->getCharacterName(), pProfile->getServer(), pProfile->getClass(), pProfile->getItemLevel());
            pThread->start();
        }

        emit CharacterSearch::getInstance()->updateCharacter();
        m_handlerStatus = 0x00;
    }
}

CharacterManager* CharacterManager::getInstance()
{
    if (m_pInstance == nullptr)
        m_pInstance = new CharacterManager();
    return m_pInstance;
}

void CharacterManager::destroyInstance()
{
    if (m_pInstance == nullptr)
        return;
    delete m_pInstance;
    m_pInstance = nullptr;
}

Character* CharacterManager::getCharacter(QString name)
{
    // return Character if exist, otherwise create Character
    if (m_nameToCharacter.contains(name))
        return m_nameToCharacter[name];

    m_requestedCharacterName = name;
    m_nameToCharacter[name] = new Character();
    requestCharacterInfo();
    return nullptr;
}

void CharacterManager::removeCharacter(QString name)
{
    delete m_nameToCharacter[name];
    m_nameToCharacter.remove(name);
}
