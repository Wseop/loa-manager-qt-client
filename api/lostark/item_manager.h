#ifndef ITEMMANAGER_H
#define ITEMMANAGER_H

#include "api/lostark/search_option.h"

#include <QObject>
#include <QHash>

struct MarketItem {
    QString itemName;
    QString itemGrade;
    QString iconPath;
    double avgPrice = -1;
    int recentPrice = -1;
    int minPrice = -1;
};

struct AuctionItemOption {
    QString optionName;
    QString tripod;
    int value = -1;
};

struct AuctionItem {
    QString itemName;
    QString itemGrade;
    QString iconPath;
    int quality = -1;
    QList<AuctionItemOption> itemOptions;
    int buyPrice = -1;
    int startPrice = -1;
    int currentBidPrice = -1;
    int nextBidPrice = -1;
};

class ItemManager : public QObject
{
    Q_OBJECT

private:
    ItemManager();
    ~ItemManager();

    void initializeMarketSearchOption();
    void initializeAuctionSearchOption();

    void searchMarketItem();
    void searchAuctionItem();

public:
    void refreshItemPrice();

    MarketItem getMarketItem(const QString &itemName);
    AuctionItem getAuctionItem(const QString &itemName);

private:
    bool mIsRefreshRequested;

    // search preset
    QList<MarketSearchOption> mMarketSearchOptions;
    QList<AuctionSearchOption> mAuctionSearchOptions;

    // search result
    QHash<QString, MarketItem> mMarketItemMap;
    QHash<QString, AuctionItem> mAuctionItemMap;

public:
    static ItemManager *getInstance();
    static void destroyInstance();

private:
    static ItemManager *pInstance;

};

#endif // ITEMMANAGER_H
