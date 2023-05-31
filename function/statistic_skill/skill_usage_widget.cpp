#include "skill_usage_widget.h"
#include "ui_skill_usage_widget.h"
#include "ui/widget_manager.h"
#include "resource/resource_manager.h"

#include <QLabel>
#include <QGroupBox>

SkillUsageWidget::SkillUsageWidget(const Skill &skill) :
    ui(new Ui::SkillUsageWidget),
    mSkillUsageCount(0),
    pSkillUsageRatioLabel(nullptr),
    mLevels({1, 4, 7, 10, 12}),
    pRuneLayout(nullptr)
{
    ui->setupUi(this);

    initializeTitleLayout(skill.isCounter, skill.iconPath, skill.skillName);
    initializeLevelLayout();
    initializeTripodLayout(skill.tripods);
    initializeRuneLayout();
}

SkillUsageWidget::~SkillUsageWidget()
{
    delete ui;
}

void SkillUsageWidget::updateUsageRatio(int total, const SkillUsage &skillUsage)
{
    mSkillUsageCount = skillUsage.count;

    updateSkillUsageRatio(total, skillUsage.count);
    updateLevelUsageRatio(skillUsage.count, skillUsage.levels);
    updateTripodUsageRatio(skillUsage.count, skillUsage.tripods);

    const QList<QPair<QString, int>> &runes = skillUsage.runes;
    int addedCount = 0;

    for (const QPair<QString, int> &rune : runes) {
        pRuneLayout->addLayout(createRuneUsageRatio(skillUsage.count, rune));
        addedCount++;

        if (addedCount == 5) {
            break;
        }
    }
}

int SkillUsageWidget::getSkillUsageCount()
{
    return mSkillUsageCount;
}

void SkillUsageWidget::initializeTitleLayout(bool isCounter, const QString &iconPath, const QString &skillName)
{
    if (isCounter) {
        ui->vLayoutTitle->addWidget(createCounterLabel(), 0, Qt::AlignHCenter);
    }
    ui->vLayoutTitle->addWidget(createSkillIcon(iconPath), 0, Qt::AlignHCenter);
    ui->vLayoutTitle->addWidget(createSkillNameLabel(skillName), 0, Qt::AlignHCenter);
    ui->vLayoutTitle->addWidget(createSkillUsageRatioLabel(), 0, Qt::AlignHCenter);
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

QLabel *SkillUsageWidget::createSkillUsageRatioLabel()
{
    pSkillUsageRatioLabel = WidgetManager::createLabel("-");

    return pSkillUsageRatioLabel;
}

void SkillUsageWidget::updateSkillUsageRatio(int total, int count)
{
    double ratio = count / static_cast<double>(total) * 100;

    pSkillUsageRatioLabel->setText(QString("(%1%)").arg(ratio, 0, 'f', 2));

    if (ratio >= 30) {
        pSkillUsageRatioLabel->setStyleSheet("QLabel { color: blue }");
    }
}

void SkillUsageWidget::initializeLevelLayout()
{
    ui->vLayoutMain->addWidget(createLevelGroupBox());
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

void SkillUsageWidget::updateLevelUsageRatio(int total, const QHash<int, int> &levels)
{
    for (int level : mLevels) {
        if (levels.contains(level)) {
            double ratio = levels[level] / static_cast<double>(total) * 100;

            mLevelUsageRatioLabelMap[level]->setText(
                QString("(%1%)").arg(ratio, 0, 'f', 2));

            if (ratio >= 30) {
                mLevelUsageRatioLabelMap[level]->setStyleSheet("QLabel { color: blue }");
            }
        }
    }
}

void SkillUsageWidget::initializeTripodLayout(const QList<Tripod> &tripods)
{
    for (int i = 0; i < 3; i++) {
        ui->vLayoutMain->addWidget(createTripodGroupBox(i, tripods));
    }
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

void SkillUsageWidget::updateTripodUsageRatio(int total, const QHash<QString, int> &tripods)
{
    for (auto iter = tripods.constBegin(); iter != tripods.constEnd(); iter++) {
        double ratio = iter.value() / static_cast<double>(total) * 100;

        mTripodUsageRatioLabelMap[iter.key()]->setText(
            QString("(%1%)").arg(ratio, 0, 'f', 2));

        if (ratio >= 30) {
            mTripodUsageRatioLabelMap[iter.key()]->setStyleSheet("QLabel { color: blue }");
        }
    }
}

void SkillUsageWidget::initializeRuneLayout()
{
    ui->vLayoutMain->addWidget(createRuneGroupBox());
}

QGroupBox *SkillUsageWidget::createRuneGroupBox()
{
    QGroupBox *pGroupBox = WidgetManager::createGroupBox("룬");

    pRuneLayout = new QHBoxLayout();
    pGroupBox->setLayout(pRuneLayout);

    return pGroupBox;
}

QVBoxLayout *SkillUsageWidget::createRuneUsageRatio(int total, const QPair<QString, int> &rune)
{
    QVBoxLayout *pRuneUsageLayout = new QVBoxLayout();

    pRuneUsageLayout->addWidget(createRuneIcon(rune.first), 0, Qt::AlignHCenter);
    pRuneUsageLayout->addWidget(createRuneNameLabel(rune.first), 0, Qt::AlignHCenter);
    pRuneUsageLayout->addWidget(createRuneUsageRatioLabel(total, rune.second), 0, Qt::AlignHCenter);

    return pRuneUsageLayout;
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

QLabel *SkillUsageWidget::createRuneUsageRatioLabel(int total, int count)
{
    double ratio = count / static_cast<double>(total) * 100;

    QLabel *pRuneUsageRatioLabel = WidgetManager::createLabel(
        QString("(%1%)").arg(ratio, 0, 'f', 2));

    if (ratio >= 30) {
        pRuneUsageRatioLabel->setStyleSheet("QLabel { color: blue }");
    }

    return pRuneUsageRatioLabel;
}
