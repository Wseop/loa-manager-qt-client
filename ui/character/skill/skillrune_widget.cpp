#include "skillrune_widget.h"
#include "ui_skillrune_widget.h"
#include "game_data/character/skill/skillrune.h"
#include "ui/widget_manager.h"
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
    QLabel* pIcon = WidgetManager::createIcon(m_pSkillRune->getIconPath(), m_pNetworkManager, this);
    m_labels.append(pIcon);
    ui->hLayoutRune->addWidget(pIcon);
}

void SkillRuneWidget::addLabels()
{
    QLabel* pLabelName = WidgetManager::createLabel(m_pSkillRune->getName(), LABEL_WIDTH, LABEL_HEIGHT, 10, this);
    m_labels.append(pLabelName);
    ui->hLayoutRune->addWidget(pLabelName);
}
