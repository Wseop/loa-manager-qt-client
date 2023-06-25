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
    SkillUsageWidget(const QString &className,
                     const double &count, const int skillCount,
                     const QString &skillName,
                     const QList<QPair<int, int>> &skillLevelCounts,
                     const QList<QPair<int, QString>> &tripodCounts,
                     const QList<QPair<int, QString>> &runeCounts);
    ~SkillUsageWidget();

private:
    // title layout
    void initializeTitleLayout(bool isCounter,
                               const QString &iconPath,
                               const QString &skillName,
                               const double &count, const int skillCount);
    QLabel *createCounterLabel();
    QLabel *createSkillIcon(const QString &iconPath);
    QLabel *createSkillNameLabel(const QString &skillName);
    QLabel *createSkillUsageRatioLabel(double ratio);

    // level layout
    void initializeLevelLayout(const int skillCount, const QList<QPair<int, int>> &skillLevels);
    QGroupBox *createLevelGroupBox();
    QLabel *createLevelLabel(int level);
    QLabel *createLevelUsageRatioLabel(int level);
    void setLevelUsageRatio(const int skillCount, const QList<QPair<int, int>> &skillLevels);

    // tripods layout
    void initializeTripodLayout(const QList<Tripod> &tripods, const int skillCount, const QList<QPair<int, QString>> &tripodCounts);
    QGroupBox *createTripodGroupBox(int tripodTier, const QList<Tripod> &tripods);
    QLabel *createTripodIcon(const QString &iconPath);
    QLabel *createTripodNameLabel(const QString &tripodName);
    QLabel *createTripodUsageRatioLabel(const QString &tripodName);
    void setTripodUsageRatio(const int skillCount, const QList<QPair<int, QString>> &tripods);

    // rune layout
    void initializeRuneLayout(const int skillCount, const QList<QPair<int, QString>> &runeCounts);
    QGroupBox *createRuneGroupBox();
    void createRuneUsageRatio(int skillCount, const QList<QPair<int, QString>> &runeCounts);
    QLabel *createRuneIcon(const QString &runeName);
    QLabel *createRuneNameLabel(const QString &runeName);
    QLabel *createRuneUsageRatioLabel(const int skillCount, const int runeCount);

private:
    Ui::SkillUsageWidget *ui;

    int mSkillUsageCount;

    // level usage
    QList<int> mLevels;
    QHash<int, QLabel*> mLevelUsageRatioLabelMap;

    // tripod usage
    QHash<QString, QLabel*> mTripodUsageRatioLabelMap;

    // rune usage
    QHBoxLayout *pRuneLayout;
};

#endif // SKILL_USAGE_WIDGET_H
