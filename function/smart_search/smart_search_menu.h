#ifndef SMART_SEARCH_MENU_H
#define SMART_SEARCH_MENU_H

struct Price
{
    int buyPrice;
    int bidStartPrice;
};

class SmartSearchMenu
{
public:
    SmartSearchMenu() {}
    virtual ~SmartSearchMenu() {}

    virtual void refresh() = 0;
};

#endif // SMART_SEARCH_MENU_H
