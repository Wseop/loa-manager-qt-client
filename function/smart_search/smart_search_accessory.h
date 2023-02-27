#ifndef SMART_SEARCH_ACCESSORY_H
#define SMART_SEARCH_ACCESSORY_H

#include "function/smart_search/smart_search_menu.h"

#include <QWidget>

namespace Ui {
class SmartSearchAccessory;
}

class SmartSearchAccessory : public QWidget, public SmartSearchMenu
{
    Q_OBJECT

public:
    SmartSearchAccessory(QLayout* pLayout);
    ~SmartSearchAccessory();

    void updatePrice(bool bResetPageNo) override;

private:
    Ui::SmartSearchAccessory *ui;
};

#endif // SMART_SEARCH_ACCESSORY_H
