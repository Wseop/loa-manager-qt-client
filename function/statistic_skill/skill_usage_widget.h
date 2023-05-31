#ifndef SKILL_USAGE_WIDGET_H
#define SKILL_USAGE_WIDGET_H

#include "game/skill/skill.h"
#include "api/statistics/settings_skill.h"

#include <QWidget>

class QLabel;
class QGroupBox;
class QHBoxLayout;
class QVBoxLayout;

namespace Ui {
class SkillUsageWidget;
}

class SkillUsageWidget : public QWidget
{
    Q_OBJECT

public:
    SkillUsageWidget(const Skill &skill);
    ~SkillUsageWidget();

    void updateUsageRatio(int total, const SkillUsage &skillUsage);
    int getSkillUsageCount();

private:
    // title layout
    void initializeTitleLayout(bool isCounter, const QString &iconPath, const QString &skillName);
    QLabel *createCounterLabel();
    QLabel *createSkillIcon(const QString &iconPath);
    QLabel *createSkillNameLabel(const QString &skillName);
    QLabel *createSkillUsageRatioLabel();
    void updateSkillUsageRatio(int total, int count);

    // level layout
    void initializeLevelLayout();
    QGroupBox *createLevelGroupBox();
    QLabel *createLevelLabel(int level);
    QLabel *createLevelUsageRatioLabel(int level);
    void updateLevelUsageRatio(int total, const QHash<int, int> &levels);

    // tripods layout
    void initializeTripodLayout(const QList<Tripod> &tripods);
    QGroupBox *createTripodGroupBox(int tripodTier, const QList<Tripod> &tripods);
    QLabel *createTripodIcon(const QString &iconPath);
    QLabel *createTripodNameLabel(const QString &tripodName);
    QLabel *createTripodUsageRatioLabel(const QString &tripodName);
    void updateTripodUsageRatio(int total, const QHash<QString, int> &tripods);

    // rune layout
    void initializeRuneLayout();
    QGroupBox *createRuneGroupBox();
    QVBoxLayout *createRuneUsageRatio(int total, const QPair<QString, int> &rune);
    QLabel *createRuneIcon(const QString &runeName);
    QLabel *createRuneNameLabel(const QString &runeName);
    QLabel *createRuneUsageRatioLabel(int total, int count);

private:
    Ui::SkillUsageWidget *ui;

    int mSkillUsageCount;

    // skill usage
    QLabel *pSkillUsageRatioLabel;

    // level usage
    QList<int> mLevels;
    QHash<int, QLabel*> mLevelUsageRatioLabelMap;

    // tripod usage
    QHash<QString, QLabel*> mTripodUsageRatioLabelMap;

    // rune usage
    QHBoxLayout *pRuneLayout;
};

#endif // SKILL_USAGE_WIDGET_H
