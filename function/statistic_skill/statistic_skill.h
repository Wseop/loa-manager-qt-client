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

private:
    StatisticSkill();
    ~StatisticSkill();

    // class layout
    void initializeClassLayout();
    QGroupBox *createClassSelector();
    void updateEngraveButton(const QString &className);

    // class-engrave layout
    void initializeClassEngraveLayout();
    QPushButton *createClassEngraveButton();

    void searchStatistic(const QString &classEngrave);
    void parseStatistic(QNetworkReply *pReply);
    QList<QPair<int, int>> parseSkillLevel(const QJsonArray &skillLevelCounts);
    QList<QPair<int, QString>> parseTripod(const QJsonArray &tripodCounts);
    QList<QPair<int, QString>> parseRune(const QJsonArray &runeCounts);

    void clearSkillUsageWidget();

    void enableInput(bool enable);

public:
    void refresh() override;

public:
    static StatisticSkill *getInstance();
    static void destroyInstance();

private:
    Ui::StatisticSkill *ui;

    QString mSelectedClass;
    SettingsSkill mStatisticData;

    // ui
    QComboBox *pClassSelector;
    QList<QPushButton*> mClassEngraveButtons;
    QList<SkillUsageWidget*> mSkillUsageWidgets;
    QList<QFrame*> mLines;

private:
    static StatisticSkill *pInstance;
};

#endif // STATISTIC_SKILL_H
