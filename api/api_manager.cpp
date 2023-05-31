#include "api_manager.h"

#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>

ApiManager *ApiManager::mpInstance = nullptr;

ApiManager::ApiManager() :
    mUrlBase("https://loamgr.xyz")
{

}

ApiManager::~ApiManager()
{

}

void ApiManager::get(QNetworkAccessManager *pNetworkManager, const QString &url)
{
    QNetworkRequest request;
    request.setRawHeader("accept", "applicaton/json");
    request.setUrl(QUrl(url));

    pNetworkManager->get(request);
}

void ApiManager::post(QNetworkAccessManager *pNetworkManager, const QString &url, const QByteArray &data)
{
    QNetworkRequest request;
    request.setRawHeader("accept", "application/json");
    request.setRawHeader("authorization", QString("bearer %1").arg(mAccessToken).toUtf8());
    request.setRawHeader("Content-Type", "application/json");
    request.setUrl(QUrl(url));

    pNetworkManager->post(request, data);
}

ApiManager *ApiManager::getInstance()
{
    if (mpInstance == nullptr)
        mpInstance = new ApiManager();

    return mpInstance;
}

void ApiManager::destroyInstance()
{
    if (mpInstance == nullptr)
        return;

    delete mpInstance;
    mpInstance = nullptr;
}

void ApiManager::setAccessToken(const QString &token)
{
    mAccessToken = token;
}

QString ApiManager::accessToken() const
{
    return mAccessToken;
}

void ApiManager::getInfos(QNetworkAccessManager *pNetworkManager)
{
    QString url = mUrlBase + "/info";

    get(pNetworkManager, url);
}

void ApiManager::trySignin(QNetworkAccessManager *pNetworkManager, const QString &userId, const QString &password)
{
    QString url = mUrlBase + "/user/signin";

    QJsonObject signinUser;
    signinUser.insert("userId", userId);
    signinUser.insert("password", password);

    post(pNetworkManager, url, QJsonDocument(signinUser).toJson());
}

void ApiManager::getResources(QNetworkAccessManager *pNetworkManager, Resources resource, const QString &param)
{
    QString url = mUrlBase + "/resources";

    switch (resource) {
    case Resources::Class:
        url += "/class";
        break;
    case Resources::Engrave:
        url += "/engrave";
        break;
    case Resources::Reward:
        url += "/reward";
        break;
    case Resources::Skill:
        url += "/skill";
        break;
    }

    url += QString("/%1").arg(param);

    get(pNetworkManager, url);
}

void ApiManager::getStatistics(QNetworkAccessManager *pNetworkManager, Statistics statistic, const QString &param)
{
    QString url = mUrlBase + "/statistics";

    switch (statistic) {
    case Statistics::RewardsChaos:
        url += "/rewards/chaos";
        break;
    case Statistics::RewardsGuardian:
        url += "/rewards/guardian";
        break;
    case Statistics::SettingsArmory:
        url += "/settings/armory";
        break;
    case Statistics::SettingsSkill:
        url += "/settings/skill";
        break;
    }

    url += QString("/%1").arg(param);

    get(pNetworkManager, url);
}

void ApiManager::getCharacter(QNetworkAccessManager *pNetworkManager, const QString &characterName)
{
    QString url = mUrlBase + QString("/lostark/characters/%1").arg(characterName);

    get(pNetworkManager, url);
}

void ApiManager::getSiblings(QNetworkAccessManager *pNetworkManager, const QString &characterName)
{
    QString url = mUrlBase + QString("/lostark/characters/%1/siblings").arg(characterName);

    get(pNetworkManager, url);
}

void ApiManager::getAuctionItems(QNetworkAccessManager *pNetworkManager, AuctionSearchOption searchOption)
{
    QString url = mUrlBase + "/auctions/items" + searchOption.getQuery();

    get(pNetworkManager, url);
}

void ApiManager::getMarketItems(QNetworkAccessManager *pNetworkManager, MarketSearchOption searchOption)
{
    QString url = mUrlBase + "/markets/items" + searchOption.getQuery();

    get(pNetworkManager, url);
}

