#include "character_search.h"
#include "ui_character_search.h"
#include "ui/widget_manager.h"
#include "ui/font_manager.h"
#include "api/api_manager.h"
#include "game/character/character.h"
#include "game/skill/skill_manager.h"
#include "function/character_search/character_info.h"

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

#include <algorithm>

CharacterSearch *CharacterSearch::mpInstance = nullptr;

CharacterSearch::CharacterSearch() :
    ui(new Ui::CharacterSearch),
    mpSearchButton(nullptr)
{
    ui->setupUi(this);

    initializeSignal();
    initializeInputLayout();
    initializeCharacterTab();
}

CharacterSearch::~CharacterSearch()
{
    delete ui;
}

void CharacterSearch::initializeSignal()
{
    connect(this, &CharacterSearch::searchRequested, this, &CharacterSearch::searchCharacter);
}

void CharacterSearch::initializeInputLayout()
{
    ui->hLayoutInput->setAlignment(Qt::AlignHCenter);

    // Layout 구성
    QGroupBox *pGroupInput = WidgetManager::createGroupBox("");
    ui->hLayoutInput->addWidget(pGroupInput);

    QHBoxLayout *pHLayoutInput = new QHBoxLayout();
    pHLayoutInput->setAlignment(Qt::AlignHCenter);
    pGroupInput->setLayout(pHLayoutInput);

    // 캐릭터명 입력창 추가
    mpLineEditCharacterName = WidgetManager::createLineEdit(nullptr, "캐릭터명을 입력해주세요.");
    pHLayoutInput->addWidget(mpLineEditCharacterName);

    // 캐릭터명 입력창 기능 구현
    connect(mpLineEditCharacterName, &QLineEdit::returnPressed, this, [&]()
    {
        if (mpSearchButton->isEnabled())
            searchCharacter(mpLineEditCharacterName->text());
    });

    // 검색 버튼 추가
    mpSearchButton = WidgetManager::createPushButton("검색", 10, 100, 25);
    pHLayoutInput->addWidget(mpSearchButton);

    // 검색 버튼 기능 구현
    connect(mpSearchButton, &QPushButton::released, this, [&]()
    {
        searchCharacter(mpLineEditCharacterName->text());
    });
}

void CharacterSearch::initializeCharacterTab()
{
    ui->tabCharacter->setStyleSheet("QWidget { background-color: #F0F0F0 }");
    ui->tabCharacter->setFont(FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, 10));

    connect(ui->tabCharacter, &QTabWidget::tabCloseRequested, this, [&](int index)
    {
        const QString &characterName = ui->tabCharacter->tabText(index);

        delete mCharacterInfos[characterName];
        mCharacterInfos.remove(characterName);

        delete mCharacters[characterName];
        mCharacters.remove(characterName);
    });
}

void CharacterSearch::searchCharacter(const QString &characterName)
{
    if (characterName == "")
        return;

    if (mCharacters.contains(characterName))
    {
        // 이미 검색된 캐릭터인 경우 tab만 변경
        for (int i = 0; i < ui->tabCharacter->count(); i++)
        {
            if (ui->tabCharacter->tabText(i) == characterName)
                ui->tabCharacter->setCurrentIndex(i);
        }

        mpLineEditCharacterName->clear();
        return;
    }

    mpSearchButton->setDisabled(true);

    Character *pCharacter = new Character();
    mCharacters[characterName] = pCharacter;

    // 캐릭터 검색
    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();

    connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&, characterName, pCharacter](QNetworkReply *pReply)
    {
        if (!ApiManager::getInstance()->handleStatusCode(pReply))
        {
            mpSearchButton->setEnabled(true);
            return;
        }

        QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());

        if (response.isNull())
        {
            // 존재하지 않는 캐릭터인 경우 메모리 해제 후 종료
            QMessageBox msgBox;
            msgBox.setText("캐릭터 정보가 없습니다.");
            msgBox.exec();

            delete pCharacter;
            mCharacters.remove(characterName);
            mpSearchButton->setEnabled(true);
            return;
        }

        // 결과 parsing
        const QJsonObject &object = response.object();
        const QJsonArray &equipments = object.find("equipments")->toArray();
        const QJsonArray &skills = object.find("skills")->toArray();
        const QJsonArray &gems = object.find("gems")->toArray();
        const QJsonArray &engraves = object.find("engraves")->toArray();
        const QJsonArray &cards = object.find("cards")->toArray();
        const QJsonArray &collectibles = object.find("collectibles")->toArray();

        pCharacter->setProfile(parseProfile(object.find("profile")->toObject(), false));

        for (const QJsonValue &value : equipments) {
            pCharacter->addEquipment(parseEquipment(value.toObject()));
        }

        for (const QJsonValue &value : skills) {
            pCharacter->addSkill(parseSkill(pCharacter->profile().className, value.toObject()));
        }

        for (const QJsonValue &value : gems) {
            pCharacter->addGem(parseGem(value.toObject()));
        }

        for (const QJsonValue &value : engraves) {
            pCharacter->addEngrave(parseEngrave(value.toObject()));
        }

        for (const QJsonValue &value : cards) {
            pCharacter->addCard(parseCard(value.toObject()));
        }

        for (const QJsonValue &value : collectibles) {
            pCharacter->addCollectible(parseCollectible(value.toObject()));
        }

        searchCharacterSibling(characterName, pCharacter);
    });

    ApiManager::getInstance()->getCharacter(pNetworkManager, characterName);
}

void CharacterSearch::searchCharacterSibling(const QString &characterName, Character *pCharacter)
{
    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();

    connect(pNetworkManager, &QNetworkAccessManager::finished,
            this, [&, pCharacter](QNetworkReply *pReply)
    {
        QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());

        if (!response.isNull()) {
            const QJsonArray &siblings = response.array();

            for (const QJsonValue &value : siblings) {
                pCharacter->addSiblings(parseProfile(value.toObject(), true));
            }

            addCharacter(pCharacter);
            mpLineEditCharacterName->clear();
        }

        mpSearchButton->setEnabled(true);
    });
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    ApiManager::getInstance()->getSiblings(pNetworkManager, characterName);
}

Profile CharacterSearch::parseProfile(const QJsonObject &object, bool isSibling)
{
    Profile profile;

    if (!isSibling) {
        profile.expeditionLevel = object.find("expeditionLevel")->toInt();
        profile.title = object.find("title")->toString();
        profile.guildName = object.find("guildName")->toString();
        profile.usingSkillPoint = object.find("usingSkillPoint")->toInt();
        profile.totalSkillPoint = object.find("totalSkillPoint")->toInt();

        const QJsonObject &stats = object.find("stats")->toObject();

        for (auto iter = stats.constBegin(); iter != stats.constEnd(); iter++) {
            profile.stats[iter.key()] = iter.value().toInt();
        }
    }

    profile.serverName = object.find("serverName")->toString();
    profile.characterName = object.find("characterName")->toString();
    profile.characterLevel = object.find("characterLevel")->toInt();
    profile.className = object.find("className")->toString();
    profile.itemLevel = object.find("itemLevel")->toDouble();

    return profile;
}

Equipment CharacterSearch::parseEquipment(const QJsonObject &object)
{
    Equipment equipment;

    equipment.type = object.find("type")->toString();
    equipment.name = object.find("name")->toString();
    equipment.iconPath = object.find("iconPath")->toString();
    equipment.itemGrade = strToItemGrade(object.find("itemGrade")->toString());

    if (object.value("quality") != QJsonValue::Undefined) {
        equipment.quality = object.find("quality")->toInt();
    }

    if (object.value("itemLevel") != QJsonValue::Undefined) {
        equipment.itemLevel = object.find("itemLevel")->toInt();
    }

    if (object.value("itemSet") != QJsonValue::Undefined) {
        const QJsonObject &itemSet = object.find("itemSet")->toObject();

        equipment.itemSet = {itemSet.find("setName")->toString(),
                             itemSet.find("setLevel")->toInt()};
    }

    if (object.value("elixirs") != QJsonValue::Undefined) {
        const QJsonObject &elixirs = object.find("elixirs")->toObject();

        for (auto iter = elixirs.constBegin(); iter != elixirs.constEnd(); iter++) {
            equipment.elixirs[iter.key()] = iter.value().toInt();
        }
    }

    if (object.value("abilities") != QJsonValue::Undefined) {
        const QJsonObject &abilities = object.find("abilities")->toObject();

        for (auto iter = abilities.constBegin(); iter != abilities.constEnd(); iter++) {
            equipment.abilities[iter.key()] = iter.value().toInt();
        }
    }

    if (object.value("engraves") != QJsonValue::Undefined) {
        const QJsonObject &engraves = object.find("engraves")->toObject();

        for (auto iter = engraves.constBegin(); iter != engraves.constEnd(); iter++) {
            equipment.engraves[iter.key()] = iter.value().toInt();
        }
    }

    if (object.value("braceletEffects") != QJsonValue::Undefined) {
        equipment.braceletEffects = object.find("braceletEffects")->toVariant()
                                          .toStringList();
    }

    if (object.value("isElla") != QJsonValue::Undefined) {
        equipment.isElla = object.find("isElla")->toBool();
    }

    return equipment;
}

Skill CharacterSearch::parseSkill(const QString &className, const QJsonObject &object)
{
    Skill skill = SkillManager::getInstance()->skill(
                    className,
                    object.find("skillName")->toString());

    skill.skillLevel = object.find("skillLevel")->toInt();

    const QJsonArray &tripods = object.find("tripods")->toArray();

    for (const QJsonValue &value : tripods) {
        for (Tripod &tripod : skill.tripods) {
            if (tripod.tripodName ==
                value.toObject().find("tripodName")->toString())
            {
                tripod.tripodLevel = value.toObject().find("tripodLevel")->toInt();
                tripod.isSelected = true;
                break;
            }
        }
    }

    if (!object.value("rune").isNull()) {
        const QJsonObject &rune = object.find("rune")->toObject();
        Rune *pRune = new Rune();

        pRune->runeName = rune.find("runeName")->toString();
        pRune->itemGrade = strToItemGrade(rune.find("itemGrade")->toString());
        pRune->iconPath = rune.find("iconPath")->toString();

        skill.pRune = pRune;
    }

    return skill;
}

Gem CharacterSearch::parseGem(const QJsonObject &object)
{
    Gem gem;

    gem.type = object.find("type")->toString();
    gem.gemLevel = object.find("gemLevel")->toInt();
    gem.iconPath = object.find("iconPath")->toString();
    gem.itemGrade = strToItemGrade(object.find("itemGrade")->toString());
    gem.skillName = object.find("skillName")->toString();

    return gem;
}

QPair<QString, int> CharacterSearch::parseEngrave(const QJsonObject &object)
{
    return {
        object.find("engraveName")->toString(),
        object.find("engraveLevel")->toInt()
    };
}

QPair<QString, int> CharacterSearch::parseCard(const QJsonObject &object)
{
    return {
        object.find("cardSet")->toString(),
        object.find("awaken")->toInt()
    };
}

Collectible CharacterSearch::parseCollectible(const QJsonObject &object)
{
    Collectible collectible;

    collectible.type = object.find("type")->toString();
    collectible.point = object.find("point")->toInt();
    collectible.maxPoint = object.find("maxPoint")->toInt();

    return collectible;
}

void CharacterSearch::addCharacter(Character *pCharacter)
{
    CharacterInfo *pCharacterInfo = new CharacterInfo(pCharacter);

    int index = ui->tabCharacter->addTab(
        pCharacterInfo,
        pCharacter->profile().characterName);

    ui->tabCharacter->setCurrentIndex(index);
    mCharacterInfos[pCharacter->profile().characterName] = pCharacterInfo;
}

void CharacterSearch::refresh()
{

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
