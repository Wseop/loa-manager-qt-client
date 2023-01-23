#include "skill_widget.h"
#include "ui_skill_widget.h"
#include "game_data/character/skill/skill.h"
#include "ui/widget_manager.h"
#include "ui/character/skill/tripod_widget.h"
#include "ui/character/skill/skillrune_widget.h"
#include <QLabel>
#include <QNetworkAccessManager>

SkillWidget::SkillWidget(QWidget* pParent, const Skill* pSkill) :
    QWidget(pParent),
    ui(new Ui::SkillWidget),
    m_pSkill(pSkill),
    m_pTripodWidget(nullptr),
    m_pSkillRuneWidget(nullptr),
    m_pNetworkManager(new QNetworkAccessManager())
{
    ui->setupUi(this);

    addIcon();
    addLabels();
    addTripodWidget();
    addSkillRuneWidget();
}

SkillWidget::~SkillWidget()
{
    for (QLabel* pLabel : m_labels)
        delete pLabel;
    if (m_pTripodWidget != nullptr)
        delete m_pTripodWidget;
    if (m_pSkillRuneWidget != nullptr)
        delete m_pSkillRuneWidget;
    delete m_pNetworkManager;
    delete ui;
}

void SkillWidget::addIcon()
{
    QLabel* pIcon = WidgetManager::createIcon(m_pSkill->getIconPath(), m_pNetworkManager, ICON_WIDTH, ICON_HEIGHT, this);
    m_labels.append(pIcon);
    ui->hLayoutGroupSkill->addWidget(pIcon);
}

void SkillWidget::addLabels()
{
    QString nameLevel = QString("%1 Lv.%2").arg(m_pSkill->getName()).arg(m_pSkill->getLevel());
    QLabel* pLabelNameLevel = WidgetManager::createLabel(nameLevel, LABEL_WIDTH, LABEL_HEIGHT, 10, this);
    m_labels.append(pLabelNameLevel);
    ui->hLayoutGroupSkill->addWidget(pLabelNameLevel);
}

void SkillWidget::addTripodWidget()
{
    m_pTripodWidget = new TripodWidget(this, m_pSkill->getTripods());
    ui->hLayoutGroupSkill->addWidget(m_pTripodWidget);
}

void SkillWidget::addSkillRuneWidget()
{
    const SkillRune* pSkillRune = m_pSkill->getRune();
    if (pSkillRune != nullptr)
    {
        m_pSkillRuneWidget = new SkillRuneWidget(this, pSkillRune);
        ui->hLayoutGroupSkill->addWidget(m_pSkillRuneWidget);
    }
}
