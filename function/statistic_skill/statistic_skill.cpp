#include "statistic_skill.h"
#include "ui_statistic_skill.h"

StatisticSkill *StatisticSkill::pInstance = nullptr;

StatisticSkill::StatisticSkill() :
    ui(new Ui::StatisticSkill)
{
    ui->setupUi(this);
}

StatisticSkill::~StatisticSkill()
{
    delete ui;
}

void StatisticSkill::start()
{

}

StatisticSkill *StatisticSkill::getInstance()
{
    if (pInstance == nullptr)
        pInstance = new StatisticSkill();

    return pInstance;
}

void StatisticSkill::destroyInstance()
{
    if (pInstance == nullptr)
        return;

    delete pInstance;
    pInstance = nullptr;
}
