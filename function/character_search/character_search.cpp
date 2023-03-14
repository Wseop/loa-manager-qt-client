#include "character_search.h"
#include "ui_character_search.h"
#include "ui/widget_manager.h"
#include "api/api_manager.h"
#include "game/character/character.h"
#include "game/skill/skill_manager.h"
#include "game/item/rune.h"

#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QThread>

CharacterSearch *CharacterSearch::mpInstance = nullptr;

CharacterSearch::CharacterSearch() :
    ui(new Ui::CharacterSearch),
    mParseStatus(0x00),
    mpSearchButton(nullptr)
{
    ui->setupUi(this);
    ui->hLayoutInput->setAlignment(Qt::AlignHCenter);

    initializeInputUI();
    initializeParser();
}

CharacterSearch::~CharacterSearch()
{
    mpLineEditCharacterName = nullptr;
    mpSearchButton = nullptr;

    for (QWidget *pWidget : mWidgets)
        delete pWidget;
    mWidgets.clear();

    for (QLayout *pLayout : mLayouts)
        delete pLayout;
    mLayouts.clear();

    delete ui;
}

void CharacterSearch::initializeInputUI()
{
    QGroupBox *pGroupInput = WidgetManager::createGroupBox("");
    ui->hLayoutInput->addWidget(pGroupInput);
    mWidgets << pGroupInput;

    QHBoxLayout *pHLayoutInput = new QHBoxLayout();
    pHLayoutInput->setAlignment(Qt::AlignHCenter);
    pGroupInput->setLayout(pHLayoutInput);
    mLayouts << pHLayoutInput;

    mpLineEditCharacterName = WidgetManager::createLineEdit(nullptr, "캐릭터명을 입력해주세요.");
    pHLayoutInput->addWidget(mpLineEditCharacterName);
    mWidgets << mpLineEditCharacterName;
    connect(mpLineEditCharacterName, &QLineEdit::returnPressed, this, [&](){
        if (mpSearchButton->isEnabled())
            searchCharacter(mpLineEditCharacterName->text());
    });

    mpSearchButton = WidgetManager::createPushButton("검색", 10, 100, 25);
    pHLayoutInput->addWidget(mpSearchButton);
    mWidgets << mpSearchButton;
    connect(mpSearchButton, &QPushButton::released, this, [&](){
        searchCharacter(mpLineEditCharacterName->text());
    });
}

void CharacterSearch::initializeParser()
{
    mParsers << &CharacterSearch::parseSibling;
    mParsers << &CharacterSearch::parseProfile;
    mParsers << &CharacterSearch::parseEquipment;
    mParsers << &CharacterSearch::parseSkill;
    mParsers << &CharacterSearch::parseEngrave;
    mParsers << &CharacterSearch::parseCard;
    mParsers << &CharacterSearch::parseGem;
}

void CharacterSearch::searchCharacter(const QString &characterName)
{
    if (characterName == "")
        return;

    if (mCharacters.contains(characterName))
    {
        // TODO. 해당 캐릭터로 UI 전환
        mpLineEditCharacterName->clear();
        return;
    }

    mpSearchButton->setDisabled(true);
    mParseStatus = 0x00;

    // 캐릭터 객체 생성
    Character *pCharacter = new Character();

    // 캐릭터 정보 검색
    for (int i = static_cast<int>(LostarkApi::Sibling); i <= static_cast<int>(LostarkApi::Gem); i++)
    {
        QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
        connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&, i, characterName, pCharacter](QNetworkReply *pReply){
            QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());

            if (response.isNull())
            {
                if (i == 0)
                {
                    QMessageBox msgBox;
                    msgBox.setText("캐릭터 정보가 없습니다.");
                    msgBox.exec();

                    delete pCharacter;
                    mpSearchButton->setEnabled(true);
                }
                return;
            }

            mCharacters[characterName] = pCharacter;

            QThread *pThreadParser = QThread::create(mParsers[i], pCharacter, response);
            connect(pThreadParser, &QThread::finished, pThreadParser, &QThread::deleteLater);
            pThreadParser->start();
        });
        connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

        ApiManager::getInstance()->get(pNetworkManager, static_cast<LostarkApi>(i), characterName);
    }
}

void CharacterSearch::updateParseStatus(uint8_t bit, Character *pCharacter)
{
    mParseStatus |= bit;

    if (mParseStatus == STATUS_PARSE_FINISHED)
    {
        renderCharacter(pCharacter);

        mpLineEditCharacterName->clear();
        mpSearchButton->setEnabled(true);
    }
}

void CharacterSearch::renderCharacter(Character *pCharacter)
{
    bool breakPoint = true;
}

void CharacterSearch::parseSibling(Character *pCharacter, QJsonDocument response)
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

    CharacterSearch::getInstance()->updateParseStatus((uint8_t)(1 << 0), pCharacter);
}

void CharacterSearch::parseProfile(Character *pCharacter, QJsonDocument response)
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

    CharacterSearch::getInstance()->updateParseStatus((uint8_t)(1 << 1), pCharacter);
}

void CharacterSearch::parseEquipment(Character *pCharacter, QJsonDocument response)
{
    CharacterSearch::getInstance()->updateParseStatus((uint8_t)(1 << 2), pCharacter);
}

void CharacterSearch::parseSkill(Character *pCharacter, QJsonDocument response)
{
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

    CharacterSearch::getInstance()->updateParseStatus((uint8_t)(1 << 3), pCharacter);
}

void CharacterSearch::parseEngrave(Character *pCharacter, QJsonDocument response)
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

    CharacterSearch::getInstance()->updateParseStatus((uint8_t)(1 << 4), pCharacter);
}

void CharacterSearch::parseCard(Character *pCharacter, QJsonDocument response)
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

    CharacterSearch::getInstance()->updateParseStatus((uint8_t)(1 << 5), pCharacter);
}

void CharacterSearch::parseGem(Character *pCharacter, QJsonDocument response)
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

    CharacterSearch::getInstance()->updateParseStatus((uint8_t)(1 << 6), pCharacter);
}

CharacterSearch *CharacterSearch::getInstance()
{
    if (mpInstance == nullptr)
        mpInstance = new CharacterSearch();

    return mpInstance;
}

void CharacterSearch::destroyInstance()
{
    if (mpInstance == nullptr)
        return;

    delete mpInstance;
    mpInstance = nullptr;
}

void CharacterSearch::start()
{

}
