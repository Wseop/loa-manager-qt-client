#ifndef RESPONSE_AUCTION_H
#define RESPONSE_AUCTION_H

#include <QString>
#include <QList>

struct AuctionItemOption
{
    QString type;
    QString optionName;
    QString optionNameTripod;
    int value;
    bool bPenalty;
    QString className;
};

struct AuctionItem
{
    QString name;
    QString grade;
    int tier;
    QString icon;
    int quality;

    struct
    {
        int startPrice;
        int buyPrice;
        int bidStartPrice;
    } AuctionInfo;

    QList<AuctionItemOption> options;
};

struct ResponseAuction
{
    int pageNo;
    int pageSize;
    int totalCount;

    QList<AuctionItem> items;
};

#endif // RESPONSE_AUCTION_H
