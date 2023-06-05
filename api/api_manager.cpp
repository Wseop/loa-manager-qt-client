#include "api_manager.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>

#include <QMessageBox>

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

void ApiManager::getAuctionItems(QNetworkAccessManager *pNetworkManager, const QString &searchOptionQuery)
{
    QString url = mUrlBase + "/lostark/auctions/items" + searchOptionQuery;

    get(pNetworkManager, url);
}

void ApiManager::getMarketItems(QNetworkAccessManager *pNetworkManager, const QString &searchOptionQuery)
{
    QString url = mUrlBase + "/lostark/markets/items" + searchOptionQuery;

    get(pNetworkManager, url);
}

bool ApiManager::handleStatusCode(QNetworkReply *pReply)
{
    int statusCode = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute)
                         .toInt();
    QMessageBox msgBox;

    if (statusCode == 200 || statusCode == 201) {
        return true;
    } else if (statusCode == 429) {
        msgBox.setText("API 요청 횟수 제한 - 1분뒤 재시도해주세요.");
        msgBox.exec();
        return false;
    } else if (statusCode == 503) {
        msgBox.setText("로스트아크 서버 점검중");
        msgBox.exec();
        return false;
    } else {
        msgBox.setText(QString::number(statusCode));
        msgBox.exec();
        return false;
    }
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
