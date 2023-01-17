#include "character_search.h"
#include "ui_character_search.h"
#include "game_data/character/character_manager.h"
#include "ui/character/character_widget.h"

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

void CharacterSearch::searchCharacter()
{
    m_pCharacter = CharacterManager::getInstance()->getCharacter(ui->leCharacterName->text());
    if (m_pCharacter != nullptr)
    {
        ui->leCharacterName->clear();
        clearCharacterWidget();
        m_pCharacterWidget = new CharacterWidget(this, m_pCharacter);
        ui->vLayoutCharacterSearch->addWidget(m_pCharacterWidget);
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
