#ifndef SMART_SEARCH_BRACELET_H
#define SMART_SEARCH_BRACELET_H

#include "function/smart_search/smart_search_menu.h"

#include <QWidget>

namespace Ui {
class SmartSearchBracelet;
}

class SmartSearchBracelet : public QWidget, public SmartSearchMenu
{
    Q_OBJECT

public:
    SmartSearchBracelet(QLayout* pLayout);
    ~SmartSearchBracelet();

    void updatePrice(bool bResetPageNo) override;

private:
    Ui::SmartSearchBracelet *ui;
};

#endif // SMART_SEARCH_BRACELET_H
