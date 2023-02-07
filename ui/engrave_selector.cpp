#include "engrave_selector.h"
#include "ui_engrave_selector.h"
#include "ui/font_manager.h"
#include "ui/widget_manager.h"
#include "game_data/character/engrave/engrave_manager.h"
#include <QPushButton>
#include <QScreen>
#include <functional>

EngraveSelector::EngraveSelector(bool bClassEnable) :
    ui(new Ui::EngraveSelector)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icon/Home.ico"));
    this->setWindowTitle("각인 선택");

    if (!bClassEnable)
    {
        ui->line->hide();
        ui->lbClass->hide();
    }

    setFonts();
    setAlignments();
    addSelectButtons(bClassEnable);
}

EngraveSelector::~EngraveSelector()
{
    delete ui;
}

QList<QPushButton*>& EngraveSelector::getButtons()
{
    return m_buttons;
}

void EngraveSelector::setFonts()
{
    FontManager* pFontManager = FontManager::getInstance();
    QFont nanumBold14 = pFontManager->getFont(FontFamily::NanumSquareNeoBold, 14);

    ui->lbBattle->setFont(nanumBold14);
    ui->lbClass->setFont(nanumBold14);
}

void EngraveSelector::setAlignments()
{
    ui->vLayoutBattleEngrave->setAlignment(Qt::AlignTop);
    ui->vLayoutClassEngrave->setAlignment(Qt::AlignTop);
}

void EngraveSelector::addSelectButtons(bool bClassEnable)
{
    addBattleEngraveButtons();
    if (bClassEnable)
        addClassEngraveButtons();
}

void EngraveSelector::addBattleEngraveButtons()
{
    EngraveManager* pEngraveManager = EngraveManager::getInstance();
    QStringList battleEngraves = pEngraveManager->getNormalEngraves();
    std::sort(battleEngraves.begin(), battleEngraves.end());

    for (int i = 0; i < battleEngraves.size(); i++)
    {
        const QString& engrave = battleEngraves[i];
        QPushButton* pButton = WidgetManager::createPushButton(engrave, 10, BUTTON_WIDTH, BUTTON_HEIGHT);
        m_buttons.append(pButton);
        ui->gridBattleEngrave->addWidget(pButton, i / MAX_COL, i % MAX_COL);
    }
}

void EngraveSelector::addClassEngraveButtons()
{
    EngraveManager* pEngraveManager = EngraveManager::getInstance();
    const QStringList& classEngraves = pEngraveManager->getClassEngraves();

    for (int i = 0; i < classEngraves.size(); i++)
    {
        const QString& engrave = classEngraves[i];
        QPushButton* pButton = WidgetManager::createPushButton(engrave, 10, BUTTON_WIDTH, BUTTON_HEIGHT);
        m_buttons.append(pButton);
        ui->gridClassEngrave->addWidget(pButton, i / MAX_COL, i % MAX_COL);
    }
}
