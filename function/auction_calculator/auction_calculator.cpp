#include "auction_calculator.h"
#include "ui_auction_calculator.h"
#include "ui/font_manager.h"

#include <QIntValidator>

AuctionCalculator *AuctionCalculator::mpInstance = nullptr;

AuctionCalculator::AuctionCalculator() :
    ui(new Ui::AuctionCalculator)
{
    ui->setupUi(this);
    ui->vLayoutAuctionCalculator->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    initializeInputLayout();
    initializeOutputLayout();
}

AuctionCalculator::~AuctionCalculator()
{
    delete ui;
}

void AuctionCalculator::initializeInputLayout()
{
    // set alignment
    ui->hLayoutInput->setAlignment(Qt::AlignHCenter);

    // set font
    QFont font = FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, 10);

    ui->lbMarketPrice->setFont(font);
    ui->lePrice->setFont(font);
    ui->pbCalc->setFont(font);

    // set validator
    ui->lePrice->setValidator(new QIntValidator());

    // set input operation
    connect(ui->lePrice, &QLineEdit::returnPressed, this, &AuctionCalculator::calculatePrice);
    connect(ui->pbCalc, &QPushButton::released, this, &AuctionCalculator::calculatePrice);
}

void AuctionCalculator::initializeOutputLayout()
{
    // set alignment
    ui->gridCalculator->setAlignment(Qt::AlignHCenter);

    // set font
    QFont font = FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, 10);

    ui->lb4P->setFont(font);
    ui->lb8P->setFont(font);
    ui->lbBreakEven->setFont(font);
    ui->lbRecommend->setFont(font);
    ui->resultBreakEven4->setFont(font);
    ui->resultBreakEven8->setFont(font);
    ui->resultLowest4->setFont(font);
    ui->resultLowest8->setFont(font);

    // set style
    const QString labelStyle = "QLabel { border: 1px solid black }";
    const QString resultStyle = "QLineEdit { color: red }";

    ui->lb4P->setStyleSheet(labelStyle);
    ui->lb8P->setStyleSheet(labelStyle);
    ui->lbBreakEven->setStyleSheet(labelStyle);
    ui->lbRecommend->setStyleSheet(labelStyle);
    ui->resultBreakEven4->setStyleSheet(resultStyle);
    ui->resultBreakEven8->setStyleSheet(resultStyle);
    ui->resultLowest4->setStyleSheet(resultStyle);
    ui->resultLowest8->setStyleSheet(resultStyle);
}

void AuctionCalculator::calculatePrice()
{
    double inputPrice = ui->lePrice->text().toDouble();

    int resultBreakEven4 = inputPrice * DISTRIBUTE_BALANCE_4;
    int resultBreakEven8 = inputPrice * DISTRIBUTE_BALANCE_8;
    int resultLowest4 = inputPrice * DISTRIBUTE_LOWPRICE_4;
    int resultLowest8 = inputPrice * DISTRIBUTE_LOWPRICE_8;

    QString resultText = "%L1";

    ui->resultBreakEven4->setText(resultText.arg(resultBreakEven4));
    ui->resultBreakEven8->setText(resultText.arg(resultBreakEven8));
    ui->resultLowest4->setText(resultText.arg(resultLowest4));
    ui->resultLowest8->setText(resultText.arg(resultLowest8));
}

AuctionCalculator *AuctionCalculator::getInstance()
{
    if (mpInstance == nullptr)
        mpInstance = new AuctionCalculator();

    return mpInstance;
}

void AuctionCalculator::destroyInstance()
{
    if (mpInstance == nullptr)
        return;

    delete mpInstance;
    mpInstance = nullptr;
}

void AuctionCalculator::start()
{
    // Nothing to do
}
