#ifndef AUCTION_CALCULATOR_H
#define AUCTION_CALCULATOR_H

#include "function/function_widget.h"

#include <QWidget>

namespace Ui {
class AuctionCalculator;
}

class AuctionCalculator : public QWidget, public FunctionWidget
{
    Q_OBJECT

private:
    AuctionCalculator();
    ~AuctionCalculator();

    void initializeInputLayout();
    void initializeOutputLayout();

    void calculatePrice();

public:
    static AuctionCalculator *getInstance();
    static void destroyInstance();

public:
    void start() override;

private:
    Ui::AuctionCalculator *ui;

    static AuctionCalculator *mpInstance;

    const double DISTRIBUTE_BALANCE_4 = 0.71;
    const double DISTRIBUTE_BALANCE_8 = 0.83;
    const double DISTRIBUTE_LOWPRICE_4 = 0.65;
    const double DISTRIBUTE_LOWPRICE_8 = 0.76;
};

#endif // AUCTION_CALCULATOR_H
