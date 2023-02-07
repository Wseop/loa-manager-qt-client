#include "characterlist_widget.h"
#include "ui_characterlist_widget.h"
#include "ui/widget_manager.h"
#include "function/character_search/character_search.h"
#include "function/setting_ranking/setting_ranking.h"
#include <QPushButton>
#include <functional>

CharacterListWidget::CharacterListWidget(QWidget* pParent, QList<CharacterInfo> characterInfos) :
    QWidget(pParent),
    ui(new Ui::CharacterListWidget)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icon/Home.ico"));
    this->setWindowTitle("캐릭터 목록");

    addCharacterButtons(characterInfos);
}

CharacterListWidget::~CharacterListWidget()
{
    for (QPushButton* pButton : m_buttons)
        delete pButton;
    delete ui;
}

void CharacterListWidget::addCharacterButtons(QList<CharacterInfo> characterInfos)
{
    const int BUTTON_WIDTH = 200;
    const int BUTTON_HEIGHT = 50;

    std::sort(characterInfos.begin(), characterInfos.end(), [&](CharacterInfo& a, CharacterInfo& b){
        return a.itemLevel > b.itemLevel;
    });

    for (int i = 0; i < characterInfos.size(); i++)
    {
        const CharacterInfo& characterInfo = characterInfos[i];
        QString buttonText = QString("[%1] %2").arg(characterInfo.itemLevel).arg(characterInfo.name);
        QPushButton* pButton = WidgetManager::createPushButton(buttonText, 10, BUTTON_WIDTH, BUTTON_HEIGHT);
        m_buttons.append(pButton);
        ui->gridCharacterList->addWidget(pButton, i / MAX_COL, i % MAX_COL);

        connect(pButton, &QPushButton::released, this, [&, characterInfo](){
            CharacterSearch::getInstance()->changeCharacter(characterInfo.name);
            this->close();
            SettingRanking::getInstance()->hide();
            CharacterSearch::getInstance()->show();
        });
    }
}
