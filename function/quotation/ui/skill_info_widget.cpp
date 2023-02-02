#include "skill_info_widget.h"
#include "ui_skill_info_widget.h"
#include "ui/font_manager.h"
#include "ui/widget_manager.h"
#include <QLabel>
#include <QVBoxLayout>

SkillInfoWidget::SkillInfoWidget(QWidget* pParent, const SkillInfo& skill, const QList<TripodInfo>& tripods) :
    QWidget(pParent),
    ui(new Ui::SkillInfoWidget)
{
    ui->setupUi(this);

    setFonts();
    setAlignments();
    addSkill(skill);
    addTripods(tripods);
}

SkillInfoWidget::~SkillInfoWidget()
{
    for (QLabel* pLabel : m_labels)
        delete pLabel;
    for (QBoxLayout* pLayout : m_layouts)
        delete pLayout;
    delete ui;
}

void SkillInfoWidget::setFonts()
{
    FontManager* pFontManager = FontManager::getInstance();
    QFont nanumBold10 = pFontManager->getFont(FontFamily::NanumSquareNeoBold, 10);

    ui->groupTripod1->setFont(nanumBold10);
    ui->groupTripod2->setFont(nanumBold10);
    ui->groupTripod3->setFont(nanumBold10);
}

void SkillInfoWidget::setAlignments()
{
    ui->vLayoutSkill->setAlignment(Qt::AlignVCenter);
}

void SkillInfoWidget::addSkill(const SkillInfo& skill)
{
    const QString iconPath = ":/skill/image/skill/" + skill.iconPath;

    QLabel* pSkillIcon = WidgetManager::createIcon(iconPath, nullptr, ICON_WIDTH, ICON_HEIGHT, this);
    m_labels.append(pSkillIcon);
    ui->vLayoutSkill->addWidget(pSkillIcon);
    ui->vLayoutSkill->setAlignment(pSkillIcon, Qt::AlignHCenter);

    QLabel* pSkillName = WidgetManager::createLabel(skill.name, LABEL_WIDTH, LABEL_HEIGHT, 10, this);
    m_labels.append(pSkillName);
    ui->vLayoutSkill->addWidget(pSkillName);
}

void SkillInfoWidget::addTripods(const QList<TripodInfo>& tripods)
{
    QString iconPath = ":/tripod/image/tripod/Tripod_Tier_%1_%2.png";
    QString iconBackground = "QLabel { background-color: black }";

    // 1트포
    for (int i = 0; i < 3; i++)
    {
        const TripodInfo& tripod = tripods[i];

        QVBoxLayout* pVLayout = new QVBoxLayout();
        m_layouts.append(pVLayout);
        ui->hLayoutTripod1->addLayout(pVLayout);

        QLabel* pTripodIcon = WidgetManager::createIcon(iconPath.arg(1).arg(tripod.iconIndex), nullptr, ICON_WIDTH, ICON_HEIGHT, this);
        pTripodIcon->setStyleSheet(iconBackground);
        m_labels.append(pTripodIcon);
        pVLayout->addWidget(pTripodIcon);
        pVLayout->setAlignment(pTripodIcon, Qt::AlignHCenter);

        QLabel* pTripodName = WidgetManager::createLabel(tripod.name, LABEL_WIDTH, LABEL_HEIGHT, 10, this);
        m_labels.append(pTripodName);
        pVLayout->addWidget(pTripodName);

        QLabel* pTripodPrice = WidgetManager::createLabel("-", LABEL_WIDTH, LABEL_HEIGHT, 10, this);
        m_labels.append(pTripodPrice);
        pVLayout->addWidget(pTripodPrice);
    }

    // 2트포
    for (int i = 3; i < 6; i++)
    {
        const TripodInfo& tripod = tripods[i];

        QVBoxLayout* pVLayout = new QVBoxLayout();
        pVLayout->setAlignment(Qt::AlignHCenter);
        m_layouts.append(pVLayout);
        ui->hLayoutTripod2->addLayout(pVLayout);

        QLabel* pTripodIcon = WidgetManager::createIcon(iconPath.arg(2).arg(tripod.iconIndex), nullptr, ICON_WIDTH, ICON_HEIGHT, this);
        pTripodIcon->setStyleSheet(iconBackground);
        m_labels.append(pTripodIcon);
        pVLayout->addWidget(pTripodIcon);
        pVLayout->setAlignment(pTripodIcon, Qt::AlignHCenter);

        QLabel* pTripodName = WidgetManager::createLabel(tripod.name, LABEL_WIDTH, LABEL_HEIGHT, 10, this);
        m_labels.append(pTripodName);
        pVLayout->addWidget(pTripodName);

        QLabel* pTripodPrice = WidgetManager::createLabel("-", LABEL_WIDTH, LABEL_HEIGHT, 10, this);
        m_labels.append(pTripodPrice);
        pVLayout->addWidget(pTripodPrice);
    }

    // 3트포
    for (int i = 6; i < 8; i++)
    {
        const TripodInfo& tripod = tripods[i];

        QVBoxLayout* pVLayout = new QVBoxLayout();
        pVLayout->setAlignment(Qt::AlignHCenter);
        m_layouts.append(pVLayout);
        ui->hLayoutTripod3->addLayout(pVLayout);

        QLabel* pTripodIcon = WidgetManager::createIcon(iconPath.arg(3).arg(tripod.iconIndex), nullptr, ICON_WIDTH, ICON_HEIGHT, this);
        pTripodIcon->setStyleSheet(iconBackground);
        m_labels.append(pTripodIcon);
        pVLayout->addWidget(pTripodIcon);
        pVLayout->setAlignment(pTripodIcon, Qt::AlignHCenter);

        QLabel* pTripodName = WidgetManager::createLabel(tripod.name, LABEL_WIDTH, LABEL_HEIGHT, 10, this);
        m_labels.append(pTripodName);
        pVLayout->addWidget(pTripodName);

        QLabel* pTripodPrice = WidgetManager::createLabel("-", LABEL_WIDTH, LABEL_HEIGHT, 10, this);
        m_labels.append(pTripodPrice);
        pVLayout->addWidget(pTripodPrice);
    }
}
