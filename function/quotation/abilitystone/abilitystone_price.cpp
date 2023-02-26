#include "abilitystone_price.h"
#include "ui_abilitystone_price.h"
#include "ui/font_manager.h"

AbilityStonePrice::AbilityStonePrice(QString engrave1, QString engrave2, int price) :
    ui(new Ui::AbilityStonePrice)
{
    ui->setupUi(this);
    ui->lbEngrave1->setText(engrave1);
    ui->lbEngrave2->setText(engrave2);
    ui->lbPrice->setText(QString("%L1골").arg(price));

    initFont();
}

AbilityStonePrice::~AbilityStonePrice()
{
    delete ui;
}

void AbilityStonePrice::initFont()
{
    FontManager* pFontManager = FontManager::getInstance();
    QFont nanumBold10 = pFontManager->getFont(FontFamily::NanumSquareNeoBold, 10);

    ui->groupEngrave1->setFont(nanumBold10);
    ui->groupEngrave2->setFont(nanumBold10);
    ui->groupPrice->setFont(nanumBold10);
    ui->lbEngrave1->setFont(nanumBold10);
    ui->lbEngrave2->setFont(nanumBold10);
    ui->lbPrice->setFont(nanumBold10);
}
