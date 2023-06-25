#include "skill_usage_widget.h"
#include "ui_skill_usage_widget.h"
#include "ui/widget_manager.h"
#include "resource/resource_manager.h"
#include "game/skill/skill_manager.h"

#include <QLabel>
#include <QGroupBox>

SkillUsageWidget::SkillUsageWidget(const QString &className,
                                   const double &count, const int skillCount,
                                   const QString &skillName,
                                   const QList<QPair<int, int>> &skillLevelCounts,
                                   const QList<QPair<int, QString>> &tripodCounts,
                                   const QList<QPair<int, QString>> &runeCounts) :
    ui(new Ui::SkillUsageWidget),
    mSkillUsageCount(0),
    mLevels({1, 4, 7, 10, 11, 12}),
    pRuneLayout(nullptr)
{
    ui->setupUi(this);

    const Skill &skill = SkillManager::getInstance()->skill(className, skillName);

    initializeTitleLayout(skill.isCounter,
                          skill.iconPath,
                          skill.skillName,
                          count, skillCount);
    initializeLevelLayout(skillCount, skillLevelCounts);
    initializeTripodLayout(skill.tripods, skillCount, tripodCounts);
    initializeRuneLayout(skillCount, runeCounts);
}

SkillUsageWidget::~SkillUsageWidget()
{
    delete ui;
}

void SkillUsageWidget::initializeTitleLayout(bool isCounter,
                                             const QString &iconPath,
                                             const QString &skillName,
                                             const double &count, const int skillCount)
{
    if (isCounter)
        ui->vLayoutTitle->addWidget(createCounterLabel(), 0, Qt::AlignHCenter);
    ui->vLayoutTitle->addWidget(createSkillIcon(iconPath), 0, Qt::AlignHCenter);
    ui->vLayoutTitle->addWidget(createSkillNameLabel(skillName), 0, Qt::AlignHCenter);
    ui->vLayoutTitle->addWidget(createSkillUsageRatioLabel(skillCount / count * 100), 0, Qt::AlignHCenter);
}

QLabel *SkillUsageWidget::createCounterLabel()
{
    QLabel *pCounterLabel = WidgetManager::createLabel("카운터", 10, 50);
    pCounterLabel->setStyleSheet("QLabel { border: 1px solid black;"
                                 "         border-radius: 5px; }");

    return pCounterLabel;
}

QLabel *SkillUsageWidget::createSkillIcon(const QString &iconPath)
{
    return WidgetManager::createIcon(iconPath, nullptr);
}

QLabel *SkillUsageWidget::createSkillNameLabel(const QString &skillName)
{
    return WidgetManager::createLabel(skillName);
}

QLabel *SkillUsageWidget::createSkillUsageRatioLabel(double ratio)
{
    QLabel *pSkillUsageRatioLabel = WidgetManager::createLabel(QString("(%1%)").arg(ratio, 0, 'f', 2));

    if (ratio >= 30)
        pSkillUsageRatioLabel->setStyleSheet("QLabel { color: blue }");

    return pSkillUsageRatioLabel;
}

void SkillUsageWidget::initializeLevelLayout(const int skillCount, const QList<QPair<int, int>> &skillLevels)
{
    ui->vLayoutMain->addWidget(createLevelGroupBox());

    setLevelUsageRatio(skillCount, skillLevels);
}

QGroupBox *SkillUsageWidget::createLevelGroupBox()
{
    QGroupBox *pGroupBox = WidgetManager::createGroupBox("스킬 레벨");
    pGroupBox->setFixedWidth(200);

    QVBoxLayout *pLevelLayout = new QVBoxLayout();
    pGroupBox->setLayout(pLevelLayout);

    for (int level : mLevels) {
        QHBoxLayout *pHLayout = new QHBoxLayout();
        pLevelLayout->addLayout(pHLayout);

        pHLayout->addWidget(createLevelLabel(level));
        pHLayout->addWidget(createLevelUsageRatioLabel(level));
    }

    return pGroupBox;
}

QLabel *SkillUsageWidget::createLevelLabel(int level)
{
    QLabel *pLevelLabel = WidgetManager::createLabel(QString::number(level) + " ");

    return pLevelLabel;
}

QLabel *SkillUsageWidget::createLevelUsageRatioLabel(int level)
{
    QLabel *pLevelRatioLabel = WidgetManager::createLabel("(-)");
    mLevelUsageRatioLabelMap[level] = pLevelRatioLabel;

    return pLevelRatioLabel;
}

void SkillUsageWidget::setLevelUsageRatio(const int skillCount, const QList<QPair<int, int>> &skillLevels)
{
    for (auto iter = skillLevels.constBegin(); iter != skillLevels.constEnd(); iter++)
    {
        if (mLevels.contains(iter->second))
        {
            double ratio = iter->first / static_cast<double>(skillCount) * 100;

            mLevelUsageRatioLabelMap[iter->second]->setText(
                QString("(%1%)").arg(ratio, 0, 'f', 2));

            if (ratio >= 30) {
                mLevelUsageRatioLabelMap[iter->second]->setStyleSheet(
                    "QLabel { color: blue }");
            }
        }
    }
}

void SkillUsageWidget::initializeTripodLayout(const QList<Tripod> &tripods, const int skillCount, const QList<QPair<int, QString>> &tripodCounts)
{
    for (int i = 0; i < 3; i++)
        ui->vLayoutMain->addWidget(createTripodGroupBox(i, tripods));

    setTripodUsageRatio(skillCount, tripodCounts);
}

QGroupBox *SkillUsageWidget::createTripodGroupBox(int tripodTier, const QList<Tripod> &tripods)
{
    QGroupBox *pGroupBox = WidgetManager::createGroupBox(
        QString("트라이포드 %1").arg(tripodTier + 1));

    QHBoxLayout *pTripodLayout = new QHBoxLayout();
    pGroupBox->setLayout(pTripodLayout);

    if (tripods.size() == 0)
        return pGroupBox;

    int startIndex, endIndex;

    if (tripodTier == 0) {
        startIndex = 0;
        endIndex = 2;
    } else if (tripodTier == 1) {
        startIndex = 3;
        endIndex = 5;
    } else {
        startIndex = 6;
        endIndex = 7;
    }

    for (int i = startIndex; i <= endIndex; i++) {
        QVBoxLayout *pLayout = new QVBoxLayout();
        pTripodLayout->addLayout(pLayout);

        pLayout->addWidget(createTripodIcon(tripods[i].iconPath), 0, Qt::AlignHCenter);
        pLayout->addWidget(createTripodNameLabel(tripods[i].tripodName), 0, Qt::AlignHCenter);
        pLayout->addWidget(createTripodUsageRatioLabel(tripods[i].tripodName), 0, Qt::AlignHCenter);
    }

    return pGroupBox;
}

QLabel *SkillUsageWidget::createTripodIcon(const QString &iconPath)
{
    return WidgetManager::createIcon(iconPath, nullptr, "black");
}

QLabel *SkillUsageWidget::createTripodNameLabel(const QString &tripodName)
{
    return WidgetManager::createLabel(tripodName);
}

QLabel *SkillUsageWidget::createTripodUsageRatioLabel(const QString &tripodName)
{
    QLabel *pTripodUsageRatioLabel = WidgetManager::createLabel("-");
    mTripodUsageRatioLabelMap[tripodName] = pTripodUsageRatioLabel;

    return pTripodUsageRatioLabel;
}

void SkillUsageWidget::setTripodUsageRatio(const int skillCount, const QList<QPair<int, QString>> &tripods)
{
    for (auto iter = tripods.constBegin(); iter != tripods.constEnd(); iter++)
    {
        double ratio = iter->first / static_cast<double>(skillCount) * 100;

        mTripodUsageRatioLabelMap[iter->second]->setText(
            QString("(%1%)").arg(ratio, 0, 'f', 2));

        if (ratio >= 30) {
            mTripodUsageRatioLabelMap[iter->second]->setStyleSheet("QLabel { color: blue }");
        }
    }
}

void SkillUsageWidget::initializeRuneLayout(const int skillCount, const QList<QPair<int, QString>> &runeCounts)
{
    ui->vLayoutMain->addWidget(createRuneGroupBox());

    createRuneUsageRatio(skillCount, runeCounts);
}

QGroupBox *SkillUsageWidget::createRuneGroupBox()
{
    QGroupBox *pGroupBox = WidgetManager::createGroupBox("룬");

    pRuneLayout = new QHBoxLayout();
    pGroupBox->setLayout(pRuneLayout);

    return pGroupBox;
}

void SkillUsageWidget::createRuneUsageRatio(int skillCount, const QList<QPair<int, QString>> &runeCounts)
{
    for (int i = 0; i < runeCounts.size(); i++)
    {
        if (i >= 5)
            break;

        QVBoxLayout *pRuneUsageLayout = new QVBoxLayout();

        pRuneUsageLayout->addWidget(createRuneIcon(runeCounts[i].second), 0, Qt::AlignHCenter);
        pRuneUsageLayout->addWidget(createRuneNameLabel(runeCounts[i].second), 0, Qt::AlignHCenter);
        pRuneUsageLayout->addWidget(createRuneUsageRatioLabel(skillCount, runeCounts[i].first), 0, Qt::AlignHCenter);

        pRuneLayout->addLayout(pRuneUsageLayout);
    }
}

QLabel *SkillUsageWidget::createRuneIcon(const QString &runeName)
{
    QString iconPath = ResourceManager::getInstance()->iconPath(runeName);

    return WidgetManager::createIcon(iconPath, nullptr);
}

QLabel *SkillUsageWidget::createRuneNameLabel(const QString &runeName)
{
    return WidgetManager::createLabel(runeName);
}

QLabel *SkillUsageWidget::createRuneUsageRatioLabel(const int skillCount, const int runeCount)
{
    double ratio = runeCount / static_cast<double>(skillCount) * 100;

    QLabel *pRuneUsageRatioLabel = WidgetManager::createLabel(
        QString("(%1%)").arg(ratio, 0, 'f', 2));

    if (ratio >= 30)
        pRuneUsageRatioLabel->setStyleSheet("QLabel { color: blue }");

    return pRuneUsageRatioLabel;
}
