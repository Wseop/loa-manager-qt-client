#include "smart_search_accessory.h"
#include "ui_smart_search_accessory.h"

SmartSearchAccessory::SmartSearchAccessory(QLayout* pLayout) :
    ui(new Ui::SmartSearchAccessory)
{
    ui->setupUi(this);
    pLayout->addWidget(this);
    this->hide();
}

SmartSearchAccessory::~SmartSearchAccessory()
{
    delete ui;
}

void SmartSearchAccessory::updatePrice(bool bResetPageNo)
{

}
