#include "skillrune_widget.h"
#include "ui_skillrune_widget.h"
#include "game_data/character/skill/skillrune.h"
#include "ui/widget_manager.h"
#include "ui/font_manager.h"
#include <QLabel>
#include <QNetworkAccessManager>

SkillRuneWidget::SkillRuneWidget(QWidget* pParent, const SkillRune* pSkillRune) :
    QWidget(pParent),
    ui(new Ui::SkillRuneWidget),
    m_pSkillRune(pSkillRune),
    m_pNetworkManager(new QNetworkAccessManager())
{
    ui->setupUi(this);

    addIcon();
    addLabels();
    setFonts();
}

SkillRuneWidget::~SkillRuneWidget()
{
    for (QLabel* pLabel : m_labels)
        delete pLabel;
    delete m_pNetworkManager;
    delete ui;
}

void SkillRuneWidget::addIcon()
{
    QLabel* pIcon = WidgetManager::createIcon(m_pSkillRune->getIconPath(), m_pNetworkManager, backgroundColorCode(m_pSkillRune->getGrade()));
    m_labels.append(pIcon);
    ui->hLayoutGroupSkillRune->addWidget(pIcon);
}

void SkillRuneWidget::addLabels()
{
    QLabel* pLabelName = WidgetManager::createLabel(m_pSkillRune->getName(), 10, colorCode(m_pSkillRune->getGrade()), LABEL_WIDTH, LABEL_HEIGHT);
    m_labels.append(pLabelName);
    ui->hLayoutGroupSkillRune->addWidget(pLabelName);
}

void SkillRuneWidget::setFonts()
{
    FontManager* pFontManager = FontManager::getInstance();
    QFont nanumBold10 = pFontManager->getFont(FontFamily::NanumSquareNeoBold, 10);

    ui->groupSkillRune->setFont(nanumBold10);
}
