#ifndef STATISTIC_SKILL_H
#define STATISTIC_SKILL_H

#include "function/function_widget.h"

#include <QWidget>

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

public:
    static StatisticSkill *getInstance();
    static void destroyInstance();

private:
    Ui::StatisticSkill *ui;

private:
    static StatisticSkill *pInstance;
};

#endif // STATISTIC_SKILL_H
