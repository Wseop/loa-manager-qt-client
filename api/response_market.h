#ifndef RESPONSE_MARKET_H
#define RESPONSE_MARKET_H

#include <QString>
#include <QList>

struct MarketItem
{
    QString name;
    QString grade;
    QString icon;
    double yDayAvgPrice;
    int recentPrice;
    int currentMinPrice;
};

struct ResponseMarket
{
    int pageNo;
    int pageSize;
    int totalCount;

    QList<MarketItem> items;
};

#endif // RESPONSE_MARKET_H
