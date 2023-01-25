#include "character_search.h"
#include "ui_character_search.h"
#include "game_data/character/character_manager.h"
#include "game_data/character/character.h"
#include "game_data/character/profile/profile.h"
#include "ui/character/character_widget.h"
#include "ui/font_manager.h"
#include <QMessageBox>

CharacterSearch* CharacterSearch::m_pInstance = nullptr;

CharacterSearch::CharacterSearch() :
    ui(new Ui::CharacterSearch)
{
    ui->setupUi(this);
    ui->vLayoutCharacterSearch->setAlignment(Qt::AlignHCenter);
    ui->hLayoutSearchBar->setAlignment(Qt::AlignHCenter);
    ui->tabWidget->setStyleSheet("QWidget { background-color: rgb(240, 240, 240) }");

    initConnects();
    setFonts();
}

CharacterSearch::~CharacterSearch()
{
    for (CharacterWidget* pCharacterWidget : m_characterWidgets)
        delete pCharacterWidget;
    delete ui;
}

void CharacterSearch::initConnects()
{
    connect(ui->leCharacterName, &QLineEdit::returnPressed, this, &CharacterSearch::searchCharacter);
    connect(ui->pbSearch, &QPushButton::released, this, &CharacterSearch::searchCharacter);
    connect(this, &CharacterSearch::updateCharacter, this, &CharacterSearch::searchCharacter);
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, [&](int index){
        QString tabText = ui->tabWidget->tabText(index);
        ui->tabWidget->removeTab(index);
        delete m_characterWidgets[index];
        m_characterWidgets.remove(index);
        CharacterManager::getInstance()->removeCharacter(tabText);
    });
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

    const Character* pCharacter = CharacterManager::getInstance()->getCharacter(ui->leCharacterName->text());
    if (pCharacter != nullptr)
    {
        if (pCharacter->getProfile() == nullptr)
        {
            QMessageBox msgBox;
            msgBox.setText("존재하지 않는 캐릭터입니다.");
            msgBox.exec();
            CharacterManager::getInstance()->removeCharacter(ui->leCharacterName->text());
        }
        else
        {
            CharacterWidget* pCharacterWidget = new CharacterWidget(this, pCharacter);
            int widgetIndex = ui->tabWidget->addTab(pCharacterWidget, pCharacter->getProfile()->getCharacterName());
            ui->tabWidget->setCurrentIndex(widgetIndex);
            m_characterWidgets[widgetIndex] = pCharacterWidget;
        }

        ui->leCharacterName->clear();
        ui->pbSearch->setEnabled(true);
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

void CharacterSearch::changeCharacter(QString characterName)
{
    ui->leCharacterName->setText(characterName);
    searchCharacter();
}
