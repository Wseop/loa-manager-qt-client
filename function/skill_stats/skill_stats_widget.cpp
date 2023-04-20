#include "skill_stats_widget.h"
#include "ui_skill_stats_widget.h"
#include "ui/widget_manager.h"
#include "ui/font_manager.h"
#include "game/skill/skill.h"
#include "resource/resource_manager.h"

#include <QLabel>

const int EMPHASIS_RATIO = 30;

SkillStatsWidget::SkillStatsWidget(const Skill &skill) :
    ui(new Ui::SkillStatsWidget),
    mSkillUsage(0)
{
    ui->setupUi(this);

    initializeCounterLabel(skill.isCounter());
    initializeSkillInfo(skill.iconPath(), skill.skillName());
    initializeTripodInfo(skill.tripods());
    initializeRuneInfo();
}

SkillStatsWidget::~SkillStatsWidget()
{
    delete ui;
}

void SkillStatsWidget::updateUsage(int settingCount, const SkillUsageInfo &skillUsageInfo)
{
    // 스킬 사용 수치 업데이트
    mSkillUsage = skillUsageInfo.skillUsage;
    // 트포만 없는 경우 -> 변신(아덴) 스킬 혹은 미채용 스킬
    if (skillUsageInfo.tripodUsage.size() == 0)
    {
        mSkillUsage = 0;

        // 룬까지 없는 경우 -> 변신(아덴) 스킬이 아니면서 미채용 스킬 : 우선순위 최하
        if (skillUsageInfo.runeUsage.size() == 0)
        {
            mSkillUsage = -1;
        }
    }

    updateSkillUsage(skillUsageInfo.skillUsage, settingCount);
    updateTripodUsage(skillUsageInfo.tripodUsage, skillUsageInfo.skillUsage);
    updateRuneUsage(skillUsageInfo.runeUsage, skillUsageInfo.skillUsage);
}

int SkillStatsWidget::getSkillUsage()
{
    return mSkillUsage;
}

void SkillStatsWidget::initializeCounterLabel(bool bCounter)
{
    QFont font = FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, 10);

    ui->labelCounter->setFont(font);
    ui->labelCounter->setFixedSize(50, 25);

    if (bCounter)
    {
        ui->labelCounter->setText("카운터");
        ui->labelCounter->setStyleSheet("QLabel { border: 1px solid black;"
                                        "         border-radius: 5px; }");
    }
    else
    {
        ui->labelCounter->setText("");
        ui->labelCounter->setStyleSheet("QLabel {}");
    }
}

void SkillStatsWidget::initializeSkillInfo(const QString &iconPath, const QString &skillName)
{
    ui->vLayoutSkillInfo->setAlignment(Qt::AlignCenter);

    // 스킬 아이콘 추가
    QLabel *pSkillIcon = WidgetManager::createIcon(iconPath, nullptr);
    ui->vLayoutSkillInfo->addWidget(pSkillIcon);
    ui->vLayoutSkillInfo->setAlignment(pSkillIcon, Qt::AlignHCenter);

    // 스킬명 레이블 추가
    QLabel *pLabelSkillName = WidgetManager::createLabel(skillName, 10, "", 150);
    ui->vLayoutSkillInfo->addWidget(pLabelSkillName);

    // 스킬 이용률 레이블 추가
    mpSkillUsageLabel = WidgetManager::createLabel("-", 10, "", 150);
    ui->vLayoutSkillInfo->addWidget(mpSkillUsageLabel);
}

void SkillStatsWidget::initializeTripodInfo(const QList<Tripod> &tripods)
{
    QFont font = FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, 10);

    ui->groupTripod1->setFont(font);
    ui->groupTripod2->setFont(font);
    ui->groupTripod3->setFont(font);

    QList<QHBoxLayout*> tripodLayouts = {ui->hLayoutTripod1, ui->hLayoutTripod2, ui->hLayoutTripod3};

    // 트라이포드 정보 추가
    for (int i = 0; i < tripods.size(); i++)
    {
        QHBoxLayout *pTripodHLayout = tripodLayouts[i / 3];

        QVBoxLayout *pTripodVLayout = new QVBoxLayout();
        pTripodHLayout->addLayout(pTripodVLayout);

        QLabel *pTripodIcon = WidgetManager::createIcon(tripods[i].iconPath(), nullptr, "#000000");
        pTripodVLayout->addWidget(pTripodIcon);
        pTripodVLayout->setAlignment(pTripodIcon, Qt::AlignHCenter);

        QLabel *pLabelTripodName = WidgetManager::createLabel(tripods[i].tripodName());
        pTripodVLayout->addWidget(pLabelTripodName);

        QLabel *pLabelTripodUsage = WidgetManager::createLabel("-");
        pTripodVLayout->addWidget(pLabelTripodUsage);
        mTripodUsageLabels[tripods[i].tripodName()] = pLabelTripodUsage;
    }

    if (tripods.size() == 0)
    {
        for (QHBoxLayout *pTripodLayout : tripodLayouts)
        {
            QLabel *pLabelInfo = WidgetManager::createLabel("트라이포드 없음");
            pTripodLayout->addWidget(pLabelInfo);
        }
    }
}

void SkillStatsWidget::initializeRuneInfo()
{
    QFont font = FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, 10);

    ui->groupRune->setFont(font);
}

void SkillStatsWidget::updateSkillUsage(int skillUsage, int settingCount)
{
    double skillUsageRatio = skillUsage / static_cast<double>(settingCount) * 100;

    mpSkillUsageLabel->setText(QString("[%1%]").arg(skillUsageRatio, 0, 'f', 2));

    if (skillUsageRatio >= EMPHASIS_RATIO)
    {
        mpSkillUsageLabel->setStyleSheet(QString("QLabel { color: blue }"));
    }
    else
    {
        mpSkillUsageLabel->setStyleSheet(QString("QLabel { color: black }"));
    }
}

void SkillStatsWidget::updateTripodUsage(const QList<QPair<QString, int>> &tripodUsages, int settingCount)
{
    // 트포가 없는 스킬(변신 스킬 등)은 사용 비율 미제공
    if (tripodUsages.size() == 0)
    {
        mpSkillUsageLabel->setText(QString("-"));
        mpSkillUsageLabel->setStyleSheet(QString("QLabel {}"));
    }

    // 트라이포드 usage 초기화
    for (auto iter = mTripodUsageLabels.begin(); iter != mTripodUsageLabels.end(); iter++)
    {
        iter.value()->setText("-");
        iter.value()->setStyleSheet("QLabel {} ");
    }

    // 트라이포드 usage 업데이트
    for (auto &tripodUsage : tripodUsages)
    {
        double tripodUsageRatio = tripodUsage.second / static_cast<double>(settingCount) * 100;

        mTripodUsageLabels[tripodUsage.first]->setText(QString("[%1%]").arg(tripodUsageRatio, 0, 'f', 2));

        if (tripodUsageRatio >= EMPHASIS_RATIO)
        {
            mTripodUsageLabels[tripodUsage.first]->setStyleSheet(QString("QLabel { color: blue }"));
        }
        else
        {
            mTripodUsageLabels[tripodUsage.first]->setStyleSheet(QString("QLabel { color: black }"));
        }
    }
}

void SkillStatsWidget::updateRuneUsage(const QList<QPair<QString, int>> &runeUsages, int settingCount)
{
    static ResourceManager *pResourceManager = ResourceManager::getInstance();

    // 룬 usage 초기화
    for (QLabel *pLabel : mRuneLabels)
    {
        delete pLabel;
    }
    mRuneLabels.clear();

    for (QVBoxLayout *pLayout : mRuneLayouts)
    {
        delete pLayout;
    }
    mRuneLayouts.clear();

    // 룬 usage 업데이트 (max 5개)
    for (int i = 0; i < 5 && i < runeUsages.size(); i++)
    {
        double runeUsageRatio = runeUsages[i].second / static_cast<double>(settingCount) * 100;

        QVBoxLayout *pRuneLayout = new QVBoxLayout();
        ui->hLayoutRune->addLayout(pRuneLayout);
        mRuneLayouts << pRuneLayout;

        QLabel *pLabelRuneIcon = WidgetManager::createIcon(pResourceManager->iconPath(runeUsages[i].first), nullptr);
        pRuneLayout->addWidget(pLabelRuneIcon);
        pRuneLayout->setAlignment(pLabelRuneIcon, Qt::AlignHCenter);
        mRuneLabels << pLabelRuneIcon;

        QLabel *pLabelRuneName = WidgetManager::createLabel(runeUsages[i].first, 10, "", 50);
        pRuneLayout->addWidget(pLabelRuneName);
        pRuneLayout->setAlignment(pLabelRuneName, Qt::AlignHCenter);
        mRuneLabels << pLabelRuneName;

        QLabel *pLabelRuneUsage = WidgetManager::createLabel(QString("[%1%]").arg(runeUsageRatio, 0, 'f', 2), 10, "", 75);
        pRuneLayout->addWidget(pLabelRuneUsage);
        pRuneLayout->setAlignment(pLabelRuneUsage, Qt::AlignHCenter);
        mRuneLabels << pLabelRuneUsage;

        if (runeUsageRatio >= EMPHASIS_RATIO)
        {
            pLabelRuneUsage->setStyleSheet(QString("QLabel { color: blue }"));
        }
        else
        {
            pLabelRuneUsage->setStyleSheet(QString("QLabel { color: black }"));
        }
    }
}
