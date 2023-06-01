#ifndef APIMANAGER_H
#define APIMANAGER_H

#include "api/resources/resources.h"
#include "api/statistics/statistics.h"
#include "api/lostark/search_option.h"

#include <QObject>

class QNetworkAccessManager;

class ApiManager : public QObject
{
    Q_OBJECT

private:
    ApiManager();
    ~ApiManager();

    void get(QNetworkAccessManager *pNetworkManager, const QString &url);
    void post(QNetworkAccessManager *pNetworkManager, const QString &url, const QByteArray &data);

public:
    static ApiManager *getInstance();
    static void destroyInstance();

public:
    void setAccessToken(const QString &token);
    QString accessToken() const;

    // info
    void getInfos(QNetworkAccessManager *pNetworkManager);

    // user
    void trySignin(QNetworkAccessManager *pNetworkManager, const QString &userId, const QString &password);

    // resources
    void getResources(QNetworkAccessManager *pNetworkManager, Resources resource, const QString &param);

    // statistics
    void getStatistics(QNetworkAccessManager *pNetworkManager, Statistics statistic, const QString &param);
    void postStatistics(QNetworkAccessManager *pNetworkManager, Statistics statistic, const QByteArray &data);

    // lostark
    void getCharacter(QNetworkAccessManager *pNetworkManager, const QString &characterName);
    void getSiblings(QNetworkAccessManager *pNetworkManager, const QString &characterName);
    void getAuctionItems(QNetworkAccessManager *pNetworkManager, const QString &searchOptionQuery);
    void getMarketItems(QNetworkAccessManager *pNetworkManager, const QString &searchOptionQuery);

private:
    static ApiManager *mpInstance;

private:
    QString mUrlBase;
    QString mAccessToken;
};

#endif // APIMANAGER_H
