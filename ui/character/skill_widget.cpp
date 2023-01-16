#include "skill_widget.h"
#include "ui_skill_widget.h"
#include "game_data/character/skill/skill.h"
#include "ui/widget_manager.h"
#include <QLabel>
#include <QNetworkAccessManager>

SkillWidget::SkillWidget(QWidget* pParent, const Skill* pSkill) :
    QWidget(pParent),
    ui(new Ui::SkillWidget),
    m_pSkill(pSkill),
    m_pNetworkManager(new QNetworkAccessManager())
{
    ui->setupUi(this);

    addIcon();
    addLabels();
}

SkillWidget::~SkillWidget()
{
    for (QLabel* pLabel : m_labels)
        delete pLabel;
    delete m_pNetworkManager;
    delete ui;
}

void SkillWidget::addIcon()
{
    QLabel* pIcon = WidgetManager::createIcon(m_pSkill->getIconPath(), m_pNetworkManager, this);
    m_labels.append(pIcon);
    ui->hLayoutSkill->addWidget(pIcon);
}

void SkillWidget::addLabels()
{
    QString nameLevel = QString("%1 Lv.%2").arg(m_pSkill->getName()).arg(m_pSkill->getLevel());
    QLabel* pLabelNameLevel = WidgetManager::createLabel(nameLevel, LABEL_WIDTH, LABEL_HEIGHT, 10, this);
    m_labels.append(pLabelNameLevel);
    ui->hLayoutSkill->addWidget(pLabelNameLevel);
}
