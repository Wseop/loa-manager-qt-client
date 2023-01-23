#include "character_search.h"
#include "ui_character_search.h"
#include "game_data/character/character_manager.h"
#include "game_data/character/character.h"
#include "ui/character/character_widget.h"
#include "ui/font_manager.h"
#include <QMessageBox>

CharacterSearch* CharacterSearch::m_pInstance = nullptr;

CharacterSearch::CharacterSearch() :
    ui(new Ui::CharacterSearch),
    m_pCharacter(nullptr),
    m_pCharacterWidget(nullptr)
{
    ui->setupUi(this);
    ui->vLayoutCharacterSearch->setAlignment(Qt::AlignHCenter);
    ui->hLayoutSearchBar->setAlignment(Qt::AlignHCenter);

    initConnects();
    setFonts();
}

CharacterSearch::~CharacterSearch()
{
    delete ui;
}

void CharacterSearch::initConnects()
{
    connect(ui->leCharacterName, &QLineEdit::returnPressed, this, &CharacterSearch::searchCharacter);
    connect(ui->pbSearch, &QPushButton::pressed, this, &CharacterSearch::searchCharacter);
    connect(this, &CharacterSearch::updateCharacter, this, &CharacterSearch::searchCharacter);
}

void CharacterSearch::setFonts()
{
    FontManager* pFontManager = FontManager::getInstance();
    QFont nanumBold10 = pFontManager->getFont(FontFamily::NanumSquareNeoBold, 10);

    ui->leCharacterName->setFont(nanumBold10);
    ui->pbSearch->setFont(nanumBold10);
}

void CharacterSearch::searchCharacter()
{
    ui->pbSearch->setDisabled(true);

    m_pCharacter = CharacterManager::getInstance()->getCharacter(ui->leCharacterName->text());
    if (m_pCharacter != nullptr)
    {
        ui->leCharacterName->clear();
        clearCharacterWidget();
        if (m_pCharacter->getProfile() == nullptr)
        {
            QMessageBox msgBox;
            msgBox.setText("존재하지 않는 캐릭터입니다.");
            msgBox.exec();
        }
        else
        {
            m_pCharacterWidget = new CharacterWidget(this, m_pCharacter);
            ui->vLayoutCharacterSearch->addWidget(m_pCharacterWidget);
        }

        ui->pbSearch->setEnabled(true);
    }
}

void CharacterSearch::clearCharacterWidget()
{
    if (m_pCharacterWidget != nullptr)
    {
        delete m_pCharacterWidget;
        m_pCharacterWidget = nullptr;
    }
}

CharacterSearch* CharacterSearch::getInstance()
{
    if (m_pInstance == nullptr)
        m_pInstance = new CharacterSearch();
    return m_pInstance;
}

void CharacterSearch::destroyInstance()
{
    if (m_pInstance == nullptr)
        return;
    delete m_pInstance;
    m_pInstance = nullptr;
}
