#include "smart_search_gem.h"
#include "ui_smart_search_gem.h"

SmartSearchGem::SmartSearchGem(QLayout* pLayout) :
    ui(new Ui::SmartSearchGem)
{
    ui->setupUi(this);
    pLayout->addWidget(this);
    this->hide();
}

SmartSearchGem::~SmartSearchGem()
{
    delete ui;
}

void SmartSearchGem::updatePrice(bool bResetPageNo)
{

}
