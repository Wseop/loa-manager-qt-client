#include "item_manager.h"
#include "api/api_manager.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

ItemManager *ItemManager::pInstance = nullptr;

ItemManager::ItemManager() :
    mIsRefreshRequested(false)
{
    initializeMarketSearchOption();
    initializeAuctionSearchOption();
}

ItemManager::~ItemManager()
{

}

void ItemManager::initializeMarketSearchOption()
{
    MarketSearchOption reforgeSearchOption(MarketCategory::강화재료);
    reforgeSearchOption.setPageAll(true);

    mMarketSearchOptions << reforgeSearchOption;
}

void ItemManager::initializeAuctionSearchOption()
{
    AuctionSearchOption gemSearchOption(AuctionCategory::보석);
    gemSearchOption.setItemName("1레벨 멸화");

    mAuctionSearchOptions << gemSearchOption;
}

void ItemManager::searchMarketItem()
{
    for (int i = 0; i < mMarketSearchOptions.size(); i++)
    {
        QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();

        connect(pNetworkManager, &QNetworkAccessManager::finished,
                pNetworkManager, &QNetworkAccessManager::deleteLater);
        connect(pNetworkManager, &QNetworkAccessManager::finished,
                this, [&](QNetworkReply *pReply)
        {
            if (!ApiManager::getInstance()->handleStatusCode(pReply))
                return;

            QJsonArray items = QJsonDocument::fromJson(pReply->readAll()).array();

            for (auto iter = items.constBegin(); iter != items.constEnd(); iter++)
            {
                const QJsonObject &item = iter->toObject();

                MarketItem marketItem;
                marketItem.itemName = item.find("itemName")->toString();
                marketItem.itemGrade = item.find("itemGrade")->toString();
                marketItem.iconPath = item.find("iconPath")->toString();
                marketItem.avgPrice = item.find("avgPrice")->toDouble();
                marketItem.recentPrice = item.find("recentPrice")->toInt();
                marketItem.minPrice = item.find("minPrice")->toInt();

                mMarketItemMap[marketItem.itemName] = marketItem;
            }
        });

        ApiManager::getInstance()->getMarketItems(
            pNetworkManager, mMarketSearchOptions[i].getQuery());
    }
}

void ItemManager::searchAuctionItem()
{
    for (int i = 0; i < mAuctionSearchOptions.size(); i++)
    {
        QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();

        connect(pNetworkManager, &QNetworkAccessManager::finished,
                pNetworkManager, &QNetworkAccessManager::deleteLater);
        connect(pNetworkManager, &QNetworkAccessManager::finished,
                this, [&](QNetworkReply *pReply)
        {
            if (!ApiManager::getInstance()->handleStatusCode(pReply))
                return;

            QJsonArray items = QJsonDocument::fromJson(pReply->readAll()).array();

            for (auto iter = items.constBegin(); iter != items.constEnd(); iter++)
            {
                const QJsonObject &item = iter->toObject();

                AuctionItem auctionItem;
                auctionItem.itemName = item.find("itemName")->toString();
                auctionItem.itemGrade = item.find("itemGrade")->toString();
                auctionItem.iconPath = item.find("iconPath")->toString();
                auctionItem.quality = item.value("quality").isNull()
                                          ? -1
                                          : item.find("quality")->toInt();
                auctionItem.buyPrice = item.find("buyPrice")->toInt();
                auctionItem.startPrice = item.find("startPrice")->toInt();
                auctionItem.currentBidPrice = item.find("currentBidPrice")->toInt();
                auctionItem.nextBidPrice = item.find("nextBidPrice")->toInt();

                const QJsonArray &itemOptions = item.find("itemOptions")->toArray();

                for (auto iterOption = itemOptions.constBegin(); iterOption != itemOptions.constEnd(); iterOption++)
                {
                    const QJsonObject &itemOption = iterOption->toObject();

                    AuctionItemOption auctionItemOption;
                    auctionItemOption.optionName = itemOption.find("optionName")
                                                       ->toString();
                    auctionItemOption.tripod = itemOption.find("tripod")->toString();
                    auctionItemOption.value = itemOption.find("value")->toInt();

                    auctionItem.itemOptions << auctionItemOption;
                }

                mAuctionItemMap[auctionItem.itemName] = auctionItem;
            }
        });

        ApiManager::getInstance()->getAuctionItems(
            pNetworkManager, mAuctionSearchOptions[i].getQuery());
    }
}

void ItemManager::refreshItemPrice()
{
    if (!mIsRefreshRequested)
    {
        // TODO. when release?
        mIsRefreshRequested = true;

        mMarketItemMap.clear();
        mAuctionItemMap.clear();

        searchMarketItem();
        searchAuctionItem();
    }
}

MarketItem ItemManager::getMarketItem(const QString &itemName)
{
    if (mMarketItemMap.contains(itemName))
        return mMarketItemMap[itemName];
    else
        return {};
}

AuctionItem ItemManager::getAuctionItem(const QString &itemName)
{
    if (mAuctionItemMap.contains(itemName))
        return mAuctionItemMap[itemName];
    else
        return {};
}

ItemManager *ItemManager::getInstance()
{
    if (pInstance == nullptr)
        pInstance = new ItemManager();

    return pInstance;
}

void ItemManager::destroyInstance()
{
    if (pInstance == nullptr)
        return;

    delete pInstance;
    pInstance = nullptr;
}
