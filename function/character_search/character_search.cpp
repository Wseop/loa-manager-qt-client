#include "character_search.h"
#include "ui_character_search.h"
#include "ui/widget_manager.h"
#include "api/api_manager.h"
#include "api/response_parser.h"
#include "game/character/character.h"

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
    mParsers << &ResponseParser::parseSibling;
    mParsers << &ResponseParser::parseProfile;
    mParsers << &ResponseParser::parseEquipment;
    mParsers << &ResponseParser::parseSkill;
    mParsers << &ResponseParser::parseEngrave;
    mParsers << &ResponseParser::parseCard;
    mParsers << &ResponseParser::parseGem;
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

            QThread *pThreadParser = QThread::create(mParsers[i], response, pCharacter, static_cast<uint8_t>(1 << i));
            connect(pThreadParser, &QThread::finished, pThreadParser, &QThread::deleteLater);
            pThreadParser->start();
        });
        connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

        ApiManager::getInstance()->get(pNetworkManager, static_cast<LostarkApi>(i), characterName);
    }
}

void CharacterSearch::renderCharacter(Character *pCharacter)
{
    bool breakPoint = true;
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
