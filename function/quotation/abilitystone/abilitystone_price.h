#ifndef ABILITYSTONE_PRICE_H
#define ABILITYSTONE_PRICE_H

#include <QWidget>

namespace Ui {
class AbilityStonePrice;
}

class AbilityStonePrice : public QWidget
{
    Q_OBJECT

public:
    AbilityStonePrice(QString engrave1, QString engrave2, int price);
    ~AbilityStonePrice();

private:
    void setFonts();

private:
    Ui::AbilityStonePrice *ui;
};

#endif // ABILITYSTONE_PRICE_H
