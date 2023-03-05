#include "auction_calculator.h"
#include "ui_auction_calculator.h"
#include "ui/font_manager.h"

#include <QIntValidator>

AuctionCalculator* AuctionCalculator::m_pInstance = nullptr;

AuctionCalculator::AuctionCalculator() :
    ui(new Ui::AuctionCalculator),
    m_pInputValidator(new QIntValidator())
{
    ui->setupUi(this);
    ui->lePrice->setValidator(m_pInputValidator);

    setFonts();
    setStyleSheets();
    setAlignments();
    initConnects();
}

AuctionCalculator::~AuctionCalculator()
{
    delete ui;
}

void AuctionCalculator::setFonts()
{
    FontManager* pFontManager = FontManager::getInstance();
    QFont nanumBold10 = pFontManager->getFont(FontFamily::NanumSquareNeoBold, 10);

    ui->lbMarketPrice->setFont(nanumBold10);
    ui->lePrice->setFont(nanumBold10);
    ui->pbCalc->setFont(nanumBold10);
    ui->lb4P->setFont(nanumBold10);
    ui->lb8P->setFont(nanumBold10);
    ui->lbBreakEven->setFont(nanumBold10);
    ui->lbRecommend->setFont(nanumBold10);
    ui->resultBreakEven4->setFont(nanumBold10);
    ui->resultBreakEven8->setFont(nanumBold10);
    ui->resultLowest4->setFont(nanumBold10);
    ui->resultLowest8->setFont(nanumBold10);
}

void AuctionCalculator::setStyleSheets()
{
    QString labelStyle = "QLabel { border: 1px solid black }";
    QString resultStyle = "QLineEdit { color: red }";

    ui->lb4P->setStyleSheet(labelStyle);
    ui->lb8P->setStyleSheet(labelStyle);
    ui->lbBreakEven->setStyleSheet(labelStyle);
    ui->lbRecommend->setStyleSheet(labelStyle);
    ui->resultBreakEven4->setStyleSheet(resultStyle);
    ui->resultBreakEven8->setStyleSheet(resultStyle);
    ui->resultLowest4->setStyleSheet(resultStyle);
    ui->resultLowest8->setStyleSheet(resultStyle);
}

void AuctionCalculator::setAlignments()
{
    ui->vLayoutAuctionCalculator->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    ui->gridCalculator->setAlignment(Qt::AlignHCenter);
    ui->hLayoutInput->setAlignment(Qt::AlignHCenter);
}

void AuctionCalculator::initConnects()
{
    connect(ui->lePrice, &QLineEdit::returnPressed, this, &AuctionCalculator::calculatePrice);
    connect(ui->pbCalc, &QPushButton::released, this, &AuctionCalculator::calculatePrice);
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

AuctionCalculator* AuctionCalculator::getInstance()
{
    if (m_pInstance == nullptr)
        m_pInstance = new AuctionCalculator();
    return m_pInstance;
}

void AuctionCalculator::destroyInstance()
{
    if (m_pInstance == nullptr)
        return;
    delete m_pInstance;
    m_pInstance = nullptr;
}

void AuctionCalculator::start()
{
    // Nothing to do
}
