#ifndef STATISTIC_SKILL_H
#define STATISTIC_SKILL_H

#include "function/function_widget.h"
#include "api/statistics/settings_skill.h"

#include <QWidget>

class QGroupBox;
class QComboBox;
class QFrame;

class SkillUsageWidget;

namespace Ui {
class StatisticSkill;
}

class StatisticSkill : public QWidget, public FunctionWidget
{
    Q_OBJECT

public:
    StatisticSkill();
    ~StatisticSkill();

    void start() override;

private:
    // class layout
    void initializeClassLayout();
    QGroupBox *createClassSelector();
    QPushButton *createSearchButton();

    // class-engrave layout
    void initializeClassEngraveLayout();
    QPushButton *createClassEngraveButton();
    void setClassEngraveButtonText(int index, const QString &classEngrave);

    void searchStatistic();
    void parseStatistic(QNetworkReply *pReply);

    void setSkillUsageWidget(const QString &classEngraveName);
    void clearSkillUsageWidget();
    void updateSkillUsageRatio(const QString &classEngraveName);
    void showSkillUsageWidget();

    void enableInput(bool enable);

public:
    static StatisticSkill *getInstance();
    static void destroyInstance();

private:
    Ui::StatisticSkill *ui;

    QString mSelectedClassName;
    SettingsSkill mStatisticData;

    // ui
    QComboBox *pClassSelector;
    QPushButton *pSearchButton;
    QList<QPushButton*> mClassEngraveButtons;
    QHash<QString, SkillUsageWidget*> mSkillUsageWidgetMap;
    QList<SkillUsageWidget*> mSkillUsageWidgets;
    QList<QFrame*> mLines;

private:
    static StatisticSkill *pInstance;
};

#endif // STATISTIC_SKILL_H
