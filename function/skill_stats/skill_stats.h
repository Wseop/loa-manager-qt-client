#ifndef SKILL_STATS_H
#define SKILL_STATS_H

#include "function/function_widget.h"

#include <QWidget>

class QComboBox;
class QPushButton;
class QFrame;
class SkillStatsWidget;
class QNetworkReply;

struct SkillUsageInfo
{
    int skillUsage;
    QList<QPair<QString, int>> tripodUsage;
    QList<QPair<QString, int>> runeUsage;
};

namespace Ui {
class SkillStats;
}

class SkillStats : public QWidget, public FunctionWidget
{
    Q_OBJECT

private:
    SkillStats();
    ~SkillStats();

    void initializeMenuLayout();
    void initializeClassSelector();
    void initializeSearchButton();

    void initializeInfoLabel();

    void initializeClassEngraveSelector();
    void setClassEngraveIcon(const QString &className);

    void initializeSkillLayout(const QString &className);
    void searchSkillSettings(const QString &className);
    void updateSkillSettings(QNetworkReply *pReply);

public:
    void start() override;

public:
    static SkillStats *getInstance();
    static void destroyInstance();

private:
    Ui::SkillStats *ui;

    QPushButton *mpSearchButton;
    QComboBox *mpClassSelector;
    QList<QPushButton*> mClassEngraveSelectors;
    QHash<QString, int> mClassEngraveIndex;

    QList<int> mSettingCounts;

    // 스킬, 트라이포드, 룬 Usage
    QList<QHash<QString, SkillUsageInfo>> mSkillUsageInfos;

    // 스킬 정보 widget
    QList<QPair<QString, SkillStatsWidget*>> mSkillStatsWidgets;
    QList<QFrame*> mLines;

private:
    static SkillStats *mpInstance;
};

#endif // SKILL_STATS_H
