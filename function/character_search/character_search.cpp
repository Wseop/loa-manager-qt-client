#include "character_search.h"
#include "ui_character_search.h"
#include "ui/widget_manager.h"
#include "ui/font_manager.h"
#include "api/api_manager.h"
#include "api/lostark/lostark_api.h"
#include "api/lostark/response_parser.h"
#include "game/character/character.h"
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
    connect(mpLineEditCharacterName, &QLineEdit::returnPressed, this, [&](){
        if (mpSearchButton->isEnabled())
            searchCharacter(mpLineEditCharacterName->text());
    });

    // 검색 버튼 추가
    mpSearchButton = WidgetManager::createPushButton("검색", 10, 100, 25);
    pHLayoutInput->addWidget(mpSearchButton);
    mWidgets << mpSearchButton;

    connect(mpSearchButton, &QPushButton::released, this, [&](){
        searchCharacter(mpLineEditCharacterName->text());
    });
}

void CharacterSearch::initializeCharacterTab()
{
    ui->tabCharacter->setStyleSheet("QWidget { background-color: #F0F0F0 }");
    ui->tabCharacter->setFont(FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, 10));

    // 캐릭터 탭 close시 해당 캐릭터의 메모리 release
    connect(ui->tabCharacter, &QTabWidget::tabCloseRequested, this, [&](int index){
        const QString &characterName = ui->tabCharacter->tabText(index);

        delete mCharacterInfos[characterName];
        mCharacterInfos.remove(characterName);

        delete mCharacters[characterName];
        mCharacters.remove(characterName);
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

    // 캐릭터 검색 시작
    for (int i = static_cast<int>(LostarkApi::Sibling); i <= static_cast<int>(LostarkApi::Gem); i++)
    {
        QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
        connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&, i, characterName, pCharacter](QNetworkReply *pReply){
            QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());

            if (response.isNull())
            {
                // sibling 결과가 null이면 존재하지 않는 캐릭터
                // 객체 삭제 후 결과 parsing 중지
                if (i == 0)
                {
                    QMessageBox msgBox;
                    msgBox.setText("캐릭터 정보가 없습니다.");
                    msgBox.exec();

                    delete mCharacters[characterName];
                    mCharacters.remove(characterName);
                    mpSearchButton->setEnabled(true);
                    return;
                }

                // 존재하는 캐릭터이지만 특정 검색 결과가 존재하지 않는 경우
                // parsing 없이 진행 상태만 업데이트
                updateParseStatus(static_cast<uint8_t>(1 << i), pCharacter);
                return;
            }

            QThread *pThreadParser = QThread::create(mParsers[i], response, pCharacter);
            connect(pThreadParser, &QThread::finished, this, [&, i, pCharacter](){
                updateParseStatus(static_cast<uint8_t>(1 << i), pCharacter);
            });
            connect(pThreadParser, &QThread::finished, pThreadParser, &QThread::deleteLater);
            pThreadParser->start();
        });
        connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

        ApiManager::getInstance()->get(pNetworkManager, ApiType::Lostark, i, characterName);
    }
}

void CharacterSearch::renderCharacter(Character *pCharacter)
{
    const QString &characterName = pCharacter->getProfile()->getCharacterName();
    CharacterInfo *pCharacterInfo = new CharacterInfo(pCharacter);

    // 캐릭터 탭 추가
    int index = ui->tabCharacter->addTab(pCharacterInfo, characterName);
    ui->tabCharacter->setCurrentIndex(index);
    mCharacterInfos[characterName] = pCharacterInfo;

    mpLineEditCharacterName->clear();
    mpSearchButton->setEnabled(true);
}

void CharacterSearch::start()
{

}

void CharacterSearch::updateParseStatus(uint8_t bit, Character *pCharacter)
{
    mParseStatus |= bit;

    if (mParseStatus == STATUS_PARSE_FINISHED)
        renderCharacter(pCharacter);
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
