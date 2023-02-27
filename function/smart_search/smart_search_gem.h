#ifndef SMART_SEARCH_GEM_H
#define SMART_SEARCH_GEM_H

#include "function/smart_search/smart_search_menu.h"

#include <QWidget>

namespace Ui {
class SmartSearchGem;
}

class SmartSearchGem : public QWidget, public SmartSearchMenu
{
    Q_OBJECT

public:
    SmartSearchGem(QLayout* pLayout);
    ~SmartSearchGem();

    void updatePrice(bool bResetPageNo) override;

private:
    Ui::SmartSearchGem *ui;
};

#endif // SMART_SEARCH_GEM_H
