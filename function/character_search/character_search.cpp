#include "character_search.h"
#include "ui_character_search.h"
#include "ui/widget_manager.h"
#include "ui/font_manager.h"
#include "api/api_manager.h"
#include "api/lostark/lostark_api.h"
#include "api/response_parser.h"
#include "api/requestbody_builder.h"
#include "game/character/character.h"
#include "game/engrave/engrave_manager.h"
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
    mParseStatus(0x00),
    mpSearchButton(nullptr)
{
    ui->setupUi(this);

    initializeSignal();
    initializeInputLayout();
    initializeCharacterTab();
    initializeParser();
}

CharacterSearch::~CharacterSearch()
{
    mpLineEditCharacterName = nullptr;
    mpSearchButton = nullptr;

    for (auto iter = mCharacters.begin(); iter != mCharacters.end(); iter++)
        delete *iter;
    mCharacters.clear();

    for (auto iter = mCharacterInfos.begin(); iter != mCharacterInfos.end(); iter++)
        delete *iter;
    mCharacterInfos.clear();

    for (QWidget *pWidget : mWidgets)
        delete pWidget;
    mWidgets.clear();

    for (auto rIter = mLayouts.rbegin(); rIter != mLayouts.rend(); rIter++)
        delete *rIter;
    mLayouts.clear();

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
    mWidgets << pGroupInput;

    QHBoxLayout *pHLayoutInput = new QHBoxLayout();
    pHLayoutInput->setAlignment(Qt::AlignHCenter);
    pGroupInput->setLayout(pHLayoutInput);
    mLayouts << pHLayoutInput;

    // 캐릭터명 입력창 추가
    mpLineEditCharacterName = WidgetManager::createLineEdit(nullptr, "캐릭터명을 입력해주세요.");
    pHLayoutInput->addWidget(mpLineEditCharacterName);
    mWidgets << mpLineEditCharacterName;

    // 캐릭터명 입력창에서 Enter 입력 시 검색 시작
    connect(mpLineEditCharacterName, &QLineEdit::returnPressed, this, [&]()
    {
        if (mpSearchButton->isEnabled())
            searchCharacter(mpLineEditCharacterName->text());
    });

    // 검색 버튼 추가
    mpSearchButton = WidgetManager::createPushButton("검색", 10, 100, 25);
    pHLayoutInput->addWidget(mpSearchButton);
    mWidgets << mpSearchButton;

    connect(mpSearchButton, &QPushButton::released, this, [&]()
    {
        searchCharacter(mpLineEditCharacterName->text());
    });
}

void CharacterSearch::initializeCharacterTab()
{
    ui->tabCharacter->setStyleSheet("QWidget { background-color: #F0F0F0 }");
    ui->tabCharacter->setFont(FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, 10));

    // 캐릭터 탭 close시 해당 캐릭터의 메모리 release
    connect(ui->tabCharacter, &QTabWidget::tabCloseRequested, this, [&](int index)
    {
        const QString &characterName = ui->tabCharacter->tabText(index);

        delete mCharacterInfos[characterName];
        mCharacterInfos.remove(characterName);

        delete mCharacters[characterName];
        mCharacters.remove(characterName);
    });
}

void CharacterSearch::initializeParser()
{
    mParsers << &ResponseParser::parseProfile;
    mParsers << &ResponseParser::parseEquipment;
    mParsers << &ResponseParser::parseSkill;
    mParsers << &ResponseParser::parseEngrave;
    mParsers << &ResponseParser::parseCard;
    mParsers << &ResponseParser::parseGem;
    mParsers << &ResponseParser::parseSibling;
}

void CharacterSearch::searchCharacter(const QString &characterName)
{
    if (characterName == "")
        return;

    if (mCharacters.contains(characterName))
    {
        // 이미 검색된 캐릭터인 경우 해당 캐릭터로 tab 변경
        for (int i = 0; i < ui->tabCharacter->count(); i++)
        {
            if (ui->tabCharacter->tabText(i) == characterName)
                ui->tabCharacter->setCurrentIndex(i);
        }

        mpLineEditCharacterName->clear();
        return;
    }

    mpSearchButton->setDisabled(true);
    mParseStatus = 0x00;

    // 캐릭터 객체 생성
    Character *pCharacter = new Character();
    mCharacters[characterName] = pCharacter;

    // 캐릭터 검색
    static const QString query = "?filters=profiles%2Bequipment%2Bcombat-skills%2Bengravings%2Bcards%2Bgems";
    static const QStringList objectKeys = {"ArmoryProfile", "ArmoryEquipment", "ArmorySkills", "ArmoryEngraving", "ArmoryCard", "ArmoryGem"};

    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();

    connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&, characterName, pCharacter](QNetworkReply *pReply)
    {
        QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());

        if (response.isNull())
        {
            // 존재하지 않는 캐릭터 -> 메세지 출력 및 캐릭터 객체 제거
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

        for (int i = 0; i < objectKeys.size(); i++)
        {
            auto value = object.find(objectKeys[i]);

            if (value->isNull())
            {
                updateParseStatus(static_cast<uint8_t>(1 << i), pCharacter);
            }
            else
            {
                QThread *pThreadParser = QThread::create(mParsers[i], value->toVariant(), pCharacter);

                connect(pThreadParser, &QThread::finished, this, [&, i, pCharacter]()
                {
                    updateParseStatus(static_cast<uint8_t>(1 << i), pCharacter);
                });
                connect(pThreadParser, &QThread::finished, pThreadParser, &QThread::deleteLater);

                pThreadParser->start();
            }
        }
    });

    ApiManager::getInstance()->get(pNetworkManager, ApiType::Lostark, static_cast<int>(LostarkApi::Character), characterName, query);

    searchCharacterSibling(characterName, pCharacter);
}

void CharacterSearch::searchCharacterSibling(const QString &characterName, Character *pCharacter)
{
    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();

    connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&, pCharacter](QNetworkReply *pReply)
    {
        QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());

        if (response.isNull())
        {
            updateParseStatus(static_cast<uint8_t>(1 << 6), pCharacter);
        }
        else
        {
            QThread *pThreadParser = QThread::create(mParsers.back(), response.toVariant(), pCharacter);

            connect(pThreadParser, &QThread::finished, this, [&, pCharacter]()
            {
                updateParseStatus(static_cast<uint8_t>(1 << 6), pCharacter);
            });
            connect(pThreadParser, &QThread::finished, pThreadParser, &QThread::deleteLater);

            pThreadParser->start();
        }
    });
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    ApiManager::getInstance()->get(pNetworkManager, ApiType::Lostark, static_cast<int>(LostarkApi::Sibling), characterName, "");
}

void CharacterSearch::renderCharacter(Character *pCharacter)
{
    const QString &characterName = pCharacter->getProfile()->getCharacterName();

    CharacterInfo *pCharacterInfo = new CharacterInfo(pCharacter);

    // 캐릭터 탭 추가
    int index = ui->tabCharacter->addTab(pCharacterInfo, characterName);

    ui->tabCharacter->setCurrentIndex(index);
    mCharacterInfos[characterName] = pCharacterInfo;
}

void CharacterSearch::updateCharacterSetting(Character *pCharacter)
{
    CharacterSetting characterSetting;

    const Profile *pProfile = pCharacter->getProfile();

    characterSetting.characterName = pProfile->getCharacterName();
    characterSetting.className = pProfile->getCharacterClass();
    characterSetting.itemLevel = pProfile->getItemLevel();

    characterSetting.itemSet = extractItemSet(pCharacter->getWeapon(), pCharacter->getArmors());
    characterSetting.engrave = extractEngrave(pCharacter->getEngrave());
    characterSetting.engraveLevel = extractEngraveLevel(pCharacter->getEngrave());
    characterSetting.ability = extractAbility(pProfile->getAbility());
    characterSetting.elixir = extractElixir(pCharacter->getArmors());

    // 세팅이 비어있으면 업데이트 X
    if (characterSetting.itemSet == "" || characterSetting.ability == "" ||
        characterSetting.engrave == "" || characterSetting.engraveLevel == "")
        return;

    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();

    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    ApiManager::getInstance()->post(pNetworkManager, ApiType::LoaManager,
                                    static_cast<int>(LoamanagerApi::PostCharacterSetting),
                                    RequestBodyBuilder::buildCharacterSettingBody(characterSetting));
}

QString CharacterSearch::extractItemSet(const Weapon *pWeapon, const QList<Armor *> &armors)
{
    QList<int> setCount(static_cast<int>(ItemSet::size) + 1, 0);

    // 무기 세트효과 카운트
    if (pWeapon == nullptr)
        return "";

    setCount[static_cast<int>(pWeapon->itemSet())]++;

    // 방어구 세트효과 카운트
    for (const Armor *pArmor : armors)
    {
        if (pArmor == nullptr)
            return "";

        setCount[static_cast<int>(pArmor->itemSet())]++;
    }

    // 카운팅된 세트효과 문자열로 변환
    QString itemSet;

    for (int i = 0; i < setCount.size(); i++)
    {
        if (setCount[i] == 0)
            continue;

        itemSet += QString("%1%2").arg(setCount[i]).arg(itemSetToQString(static_cast<ItemSet>(i)));
    }

    return itemSet;
}

QString CharacterSearch::extractEngrave(const Engrave *pEngrave)
{
    if (pEngrave == nullptr)
        return "";

    const QStringList &engraves = pEngrave->getEngraves();

    EngraveManager *pEngraveManager = EngraveManager::getInstance();
    QString ret;

    for (int level = 3; level > 0; level--)
    {
        for (const QString &engrave : engraves)
        {
            if (level == pEngrave->getEngraveLevel(engrave))
            {
                ret += QString::number(pEngraveManager->getEngraveCode(engrave));
            }
        }
    }

    return ret;
}

QString CharacterSearch::extractEngraveLevel(const Engrave *pEngrave)
{
    if (pEngrave == nullptr)
        return "";

    const QStringList &engraves = pEngrave->getEngraves();

    QString ret;

    for (int level = 3; level > 0; level--)
    {
        for (const QString &engrave : engraves)
        {
            if (level == pEngrave->getEngraveLevel(engrave))
            {
                ret += QString::number(level);
            }
        }
    }

    return ret;
}

QString CharacterSearch::extractAbility(const QHash<Ability, int> &ability)
{
    QList<QPair<Ability, int>> abilities;

    // 수치가 200이상인 특성만 추가
    for (auto iter = ability.begin(); iter != ability.end(); iter++)
    {
        if (iter.value() >= 200)
        {
            abilities.append({iter.key(), iter.value()});
        }
    }

    // 내림차순 정렬
    std::sort(abilities.begin(), abilities.end(), [&](const QPair<Ability, int> &a, const QPair<Ability, int> &b)
    {
        return a.second > b.second;
    });

    QString ret;

    for (int i = 0; i < abilities.size(); i++)
    {
        ret += abilityToQString(abilities[i].first).front();
    }

    return ret;
}

QString CharacterSearch::extractElixir(const QList<Armor *> &armors)
{
    static const QString elixirEffectHead = "질서";
    static const QString elixirEffectHand = "혼돈";

    int totalLevel = 0;
    QString head, hand;

    for (const Armor *pArmor : armors)
    {
        if (pArmor == nullptr)
            return "";

        const QList<Elixir> &elixirs = pArmor->elixirs();

        for (const Elixir &elixir : elixirs)
        {
            totalLevel += elixir.level;

            if (pArmor->armorPart() == ArmorPart::Head && elixir.effect.contains(elixirEffectHead))
            {
                head = elixir.effect.sliced(0, elixir.effect.indexOf("(") - 1);
            }
            else if (pArmor->armorPart() == ArmorPart::Hand && elixir.effect.contains(elixirEffectHand))
            {
                hand = elixir.effect.sliced(0, elixir.effect.indexOf("(") - 1);
            }
        }
    }

    if (totalLevel >= 35 && head == hand)
    {
        return head;
    }
    else
    {
        return "";
    }
}

void CharacterSearch::start()
{

}

void CharacterSearch::updateParseStatus(uint8_t bit, Character *pCharacter)
{
    mParseStatus |= bit;

    if (mParseStatus == STATUS_PARSE_FINISHED)
    {
        renderCharacter(pCharacter);
        updateCharacterSetting(pCharacter);

        mpLineEditCharacterName->clear();
        mpSearchButton->setEnabled(true);
    }
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
