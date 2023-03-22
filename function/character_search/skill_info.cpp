#include "skill_info.h"
#include "ui_skill_info.h"
#include "ui/widget_manager.h"

#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkReply>

SkillInfo::SkillInfo(const Skill *pSkill) :
    ui(new Ui::SkillInfo)
{
    ui->setupUi(this);

    initializeSkillLayout1(pSkill->iconPath(), pSkill->skillLevel());
    initializeSkillLayout2(pSkill->skillName(), pSkill->isCounter());
    initializeTripodLayout(pSkill->tripods());
    initializeRuneLayout(pSkill->rune());
}

SkillInfo::~SkillInfo()
{
    for (QWidget *pWidget : mWidgets)
        delete pWidget;
    mWidgets.clear();

    for (auto rIter = mLayouts.rbegin(); rIter != mLayouts.rend(); rIter++)
        delete *rIter;
    mLayouts.clear();

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
    mWidgets << pIcon;
}

void SkillInfo::addSkillLevelLabel(int skillLevel)
{
    QLabel *pLabelSkillLevel = WidgetManager::createLabel(QString("Lv.%1").arg(skillLevel), 10, "", 50);
    ui->vLayoutSkill1->addWidget(pLabelSkillLevel);
    mWidgets << pLabelSkillLevel;
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
    mWidgets << pLabelSkillName;
}

void SkillInfo::addCounterLabel(bool bCounter)
{
    if (bCounter == false)
        return;

    QLabel *pLabelCounter = WidgetManager::createLabel("카운터", 10, "", 50);
    pLabelCounter->setStyleSheet("QLabel { border: 1px solid black; "
                                 "         border-radius: 5px; "
                                 "         padding: 2px }");
    ui->vLayoutSkill2->addWidget(pLabelCounter);
    ui->vLayoutSkill2->setAlignment(pLabelCounter, Qt::AlignHCenter);
    mWidgets << pLabelCounter;
}

void SkillInfo::initializeTripodLayout(const QList<Tripod> &tripods)
{
    QString tripodTitle = "트라이포드 (";

    for (int i = 0; i < tripods.size(); i++)
    {
        const Tripod &tripod = tripods[i];

        if (tripod.isSelected())
        {
            if (tripod.tier() == 1)
                tripodTitle += QString::number(i + 1);
            else if (tripod.tier() == 2)
                tripodTitle += QString::number(i - 2);
            else if (tripod.tier() == 3)
                tripodTitle += QString::number(i - 5);

            addTripodInfo(tripod);
        }
    }

    if (tripods.size() == 0)
        tripodTitle = "트라이포드 (없음)";
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
    mWidgets << pLabelTripodTitle;
}

void SkillInfo::addTripodInfo(const Tripod &tripod)
{
    QVBoxLayout *pVLayout = new QVBoxLayout();
    ui->hLayoutTripod->addLayout(pVLayout);
    mLayouts << pVLayout;

    // 트라이포드 아이콘
    QLabel *pIcon = WidgetManager::createIcon(tripod.iconPath(), nullptr, "#000000");
    pVLayout->addWidget(pIcon);
    pVLayout->setAlignment(pIcon, Qt::AlignHCenter);
    mWidgets << pIcon;

    // 트라이포드 명
    QLabel *pLabelTripodName = WidgetManager::createLabel(tripod.tripodName(), 10, "");
    pLabelTripodName->setFixedWidth(100);
    pVLayout->addWidget(pLabelTripodName);
    pVLayout->setAlignment(pLabelTripodName, Qt::AlignHCenter);
    mWidgets << pLabelTripodName;

    // 트라이포드 레벨
    const QString text = tripod.maxLevel() == 1 ? "Lv.1 최대" : QString("Lv.%1").arg(tripod.tripodLevel());

    QLabel *pLabelTripodLevel = WidgetManager::createLabel(text, 10, "", 75);
    pLabelTripodLevel->setStyleSheet("QLabel { border: 1px solid black; "
                                     "         border-radius: 5px; "
                                     "         padding: 2px }");
    pVLayout->addWidget(pLabelTripodLevel);
    pVLayout->setAlignment(pLabelTripodLevel, Qt::AlignHCenter);
    mWidgets << pLabelTripodLevel;
}

void SkillInfo::initializeRuneLayout(const Rune *pRune)
{
    if (pRune == nullptr)
    {
        QLabel *pLabel = WidgetManager::createLabel("룬 미착용", 10, "", 50);
        ui->vLayoutRune->addWidget(pLabel);
        mWidgets << pLabel;
        return;
    }

    addRuneIcon(pRune->iconPath(), pRune->itemGrade());
    addRuneNameLabel(pRune->itemName(), pRune->itemGrade());
}

void SkillInfo::addRuneIcon(const QString &iconPath, ItemGrade itemGrade)
{
    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    QLabel *pIcon = WidgetManager::createIcon(iconPath, pNetworkManager, itemGradeToBGColor(itemGrade));
    ui->vLayoutRune->addWidget(pIcon);
    mWidgets << pIcon;
}

void SkillInfo::addRuneNameLabel(const QString &runeName, ItemGrade itemGrade)
{
    QLabel *pLabelRuneName = WidgetManager::createLabel(runeName, 10, itemGradeToTextColor(itemGrade), 50);
    ui->vLayoutRune->addWidget(pLabelRuneName);
    mWidgets << pLabelRuneName;
}
