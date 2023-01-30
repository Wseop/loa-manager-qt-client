#ifndef AUCTION_CALCULATOR_H
#define AUCTION_CALCULATOR_H

#include <QWidget>

namespace Ui {
class AuctionCalculator;
}

class AuctionCalculator : public QWidget
{
    Q_OBJECT

private:
    AuctionCalculator();
    ~AuctionCalculator();

    void setFonts();
    void setStyleSheets();
    void setAlignments();
    void initConnects();

    void calculatePrice();

public:
    static AuctionCalculator* getInstance();
    static void destroyInstance();

private:
    Ui::AuctionCalculator *ui;

    static AuctionCalculator* m_pInstance;

    const double DISTRIBUTE_BALANCE_4 = 0.71;
    const double DISTRIBUTE_BALANCE_8 = 0.83;
    const double DISTRIBUTE_LOWPRICE_4 = 0.65;
    const double DISTRIBUTE_LOWPRICE_8 = 0.76;

    class QIntValidator* m_pInputValidator;
};

#endif // AUCTION_CALCULATOR_H
