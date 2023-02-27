#include "smart_search_bracelet.h"
#include "ui_smart_search_bracelet.h"

SmartSearchBracelet::SmartSearchBracelet(QLayout* pLayout) :
    ui(new Ui::SmartSearchBracelet)
{
    ui->setupUi(this);
    pLayout->addWidget(this);
    this->hide();
}

SmartSearchBracelet::~SmartSearchBracelet()
{
    delete ui;
}

void SmartSearchBracelet::updatePrice(bool bResetPageNo)
{

}
