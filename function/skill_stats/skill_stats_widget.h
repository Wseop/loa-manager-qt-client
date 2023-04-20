#ifndef SKILL_STATS_WIDGET_H
#define SKILL_STATS_WIDGET_H

#include "function/skill_stats/skill_stats.h"

#include <QWidget>

class Skill;
class Tripod;
class QLabel;
class QVBoxLayout;

namespace Ui {
class SkillStatsWidget;
}

class SkillStatsWidget : public QWidget
{
    Q_OBJECT

public:
    SkillStatsWidget(const Skill &skill);
    ~SkillStatsWidget();

    void updateUsage(int settingCount, const SkillUsageInfo &skillUsageInfo);

    int getSkillUsage();

private:
    void initializeCounterLabel(bool bCounter);
    void initializeSkillInfo(const QString &iconPath, const QString &skillName);
    void initializeTripodInfo(const QList<Tripod> &tripods);
    void initializeRuneInfo();

    void updateSkillUsage(int skillUsage, int settingCount);
    void updateTripodUsage(const QList<QPair<QString, int>> &tripodUsages, int settingCount);
    void updateRuneUsage(const QList<QPair<QString, int>> &runeUsages, int settingCount);

private:
    Ui::SkillStatsWidget *ui;

    QLabel *mpSkillUsageLabel;
    QHash<QString, QLabel*> mTripodUsageLabels;

    QList<QVBoxLayout*> mRuneLayouts;
    QList<QLabel*> mRuneLabels;

    int mSkillUsage;
};

#endif // SKILL_STATS_WIDGET_H
