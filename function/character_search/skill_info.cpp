#include "skill_info.h"
#include "ui_skill_info.h"
#include "ui/widget_manager.h"

#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkReply>

SkillInfo::SkillInfo(const Skill &skill) :
    ui(new Ui::SkillInfo)
{
    ui->setupUi(this);

    initializeSkillLayout1(skill.iconPath, skill.skillLevel);
    initializeSkillLayout2(skill.skillName, skill.isCounter);
    initializeTripodLayout(skill.tripods);
    initializeRuneLayout(skill.pRune);
}

SkillInfo::~SkillInfo()
{
    delete ui;
}

void SkillInfo::initializeSkillLayout1(const QString &iconPath, int skillLevel)
{
    addSkillIcon(iconPath);
    addSkillLevelLabel(skillLevel);
}

void SkillInfo::addSkillIcon(const QString &iconPath)
{
    QLabel *pIcon = WidgetManager::createIcon(iconPath, nullptr);
    ui->vLayoutSkill1->addWidget(pIcon);
}

void SkillInfo::addSkillLevelLabel(int skillLevel)
{
    QLabel *pLabelSkillLevel = WidgetManager::createLabel(
        QString("Lv.%1").arg(skillLevel), 10, 50);

    ui->vLayoutSkill1->addWidget(pLabelSkillLevel);
}

void SkillInfo::initializeSkillLayout2(const QString &skillName, bool bCounter)
{
    addSkillNameLabel(skillName);
    addCounterLabel(bCounter);
}

void SkillInfo::addSkillNameLabel(const QString &skillName)
{
    QLabel *pLabelSkillName = WidgetManager::createLabel(skillName);
    pLabelSkillName->setFixedWidth(150);
    ui->vLayoutSkill2->addWidget(pLabelSkillName);
    ui->vLayoutSkill2->setAlignment(pLabelSkillName, Qt::AlignHCenter);
}

void SkillInfo::addCounterLabel(bool bCounter)
{
    if (bCounter == false)
        return;

    QLabel *pLabelCounter = WidgetManager::createLabel("카운터", 10, 50);
    pLabelCounter->setStyleSheet("QLabel { border: 1px solid black; "
                                 "         border-radius: 5px; "
                                 "         padding: 2px }");
    ui->vLayoutSkill2->addWidget(pLabelCounter);
    ui->vLayoutSkill2->setAlignment(pLabelCounter, Qt::AlignHCenter);
}

void SkillInfo::initializeTripodLayout(const QList<Tripod> &tripods)
{
    QString tripodTitle = "트라이포드 (";

    for (int i = 0; i < tripods.size(); i++) {
        // TODO. tripod title

        if (tripods[i].isSelected) {
            tripodTitle += QString::number((i % 3) + 1);
            addTripodInfo(tripods[i]);
        }
    }

    if (tripods.size() == 0)
        tripodTitle = "트라이포드 (-)";
    else
        tripodTitle += ")";

    addTripodTitle(tripodTitle);
}

void SkillInfo::addTripodTitle(const QString &titleText)
{
    QLabel *pLabelTripodTitle = WidgetManager::createLabel(titleText);
    pLabelTripodTitle->setFixedWidth(320);
    pLabelTripodTitle->setStyleSheet("QLabel { border: 1px solid black; "
                                     "         border-radius: 5px; "
                                     "         padding: 2px }");
    ui->hLayoutTripodTitle->addWidget(pLabelTripodTitle);
}

void SkillInfo::addTripodInfo(const Tripod &tripod)
{
    QVBoxLayout *pVLayout = new QVBoxLayout();
    ui->hLayoutTripod->addLayout(pVLayout);

    // 트라이포드 아이콘
    QLabel *pIcon = WidgetManager::createIcon(tripod.iconPath, nullptr, "#000000");
    pVLayout->addWidget(pIcon);
    pVLayout->setAlignment(pIcon, Qt::AlignHCenter);

    // 트라이포드 명
    QLabel *pLabelTripodName = WidgetManager::createLabel(tripod.tripodName, 10);
    pLabelTripodName->setFixedWidth(100);
    pVLayout->addWidget(pLabelTripodName);
    pVLayout->setAlignment(pLabelTripodName, Qt::AlignHCenter);

    // 트라이포드 레벨
    const QString text = tripod.maxLevel == 1 ?
                             "Lv.1 최대" :
                             QString("Lv.%1").arg(tripod.tripodLevel);

    QLabel *pLabelTripodLevel = WidgetManager::createLabel(text, 10, 75);
    pLabelTripodLevel->setStyleSheet("QLabel { border: 1px solid black; "
                                     "         border-radius: 5px; "
                                     "         padding: 2px }");
    pVLayout->addWidget(pLabelTripodLevel);
    pVLayout->setAlignment(pLabelTripodLevel, Qt::AlignHCenter);
}

void SkillInfo::initializeRuneLayout(const Rune *pRune)
{
    if (pRune == nullptr)
    {
        QLabel *pLabel = WidgetManager::createLabel("룬 미착용", 10, 50);
        ui->vLayoutRune->addWidget(pLabel);
        return;
    }

    addRuneIcon(pRune->iconPath, pRune->itemGrade);
    addRuneNameLabel(pRune->runeName, pRune->itemGrade);
}

void SkillInfo::addRuneIcon(const QString &iconPath, ItemGrade itemGrade)
{
    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished,
            pNetworkManager, &QNetworkAccessManager::deleteLater);

    QLabel *pIcon = WidgetManager::createIcon(
        iconPath, pNetworkManager, itemGradeToBGColor(itemGrade));

    ui->vLayoutRune->addWidget(pIcon);
}

void SkillInfo::addRuneNameLabel(const QString &runeName, ItemGrade itemGrade)
{
    QLabel *pLabelRuneName = WidgetManager::createLabel(runeName, 10, 50);
    pLabelRuneName->setStyleSheet(
        QString("QLabel { color: %1 }").arg(itemGradeToTextColor(itemGrade)));

    ui->vLayoutRune->addWidget(pLabelRuneName);
}
