#include "api_manager.h"
#include "api/loamanager/loamanager_api.h"
#include "resource/resource_manager.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

ApiManager *ApiManager::mpInstance = nullptr;

ApiManager::ApiManager() :
    mKeyIndex(0)
{
    initializeRequestUrl();
    initializeApiKey();
}

ApiManager::~ApiManager()
{

}

void ApiManager::initializeRequestUrl()
{
    const QJsonObject json = ResourceManager::getInstance()->loadJson("api");
    const QStringList &keys = json.find("List")->toVariant().toStringList();

    for (int i = 0; i < keys.size(); i++)
    {
        const QJsonArray &apis = json.find(keys[i])->toArray();

        for (const QJsonValue &value : apis)
        {
            const QJsonObject &api = value.toObject();
            const QString &url = api.find("RequestURL")->toString();

            mRequestURLs[static_cast<ApiType>(i)] << url;
        }
    }
}

void ApiManager::initializeApiKey()
{
    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();

    connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&](QNetworkReply *pReply){
        QJsonArray keys = QJsonDocument::fromJson(pReply->readAll()).array();

        for (const QJsonValue &value : keys)
        {
            mApiKeys << value.toObject().find("key")->toString();
        }
    });
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    get(pNetworkManager, ApiType::LoaManager, static_cast<int>(LoamanagerApi::ApiKey), "", "");
}

const QString &ApiManager::getApiKey()
{
    if (mKeyIndex >= mApiKeys.size())
        mKeyIndex = 0;

    return mApiKeys[mKeyIndex++];
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

void ApiManager::get(QNetworkAccessManager *pNetworkManager, ApiType apiType, int urlIndex, const QString &param, const QString &query)
{
    QString url = mRequestURLs[apiType][urlIndex];

    if (param != "")
        url = url.arg(param);

    if (query != "")
        url += query;

    QNetworkRequest request;

    if (apiType == ApiType::Lostark)
    {
        request.setRawHeader("accept", "application/json");
        request.setRawHeader("authorization", QString("bearer %1").arg(getApiKey()).toUtf8());
    }
    else if (apiType == ApiType::LoaManager)
    {
        QSslConfiguration conf = request.sslConfiguration();

        conf.setPeerVerifyMode(QSslSocket::VerifyNone);
        request.setSslConfiguration(conf);
    }
    else
        return;

    request.setUrl(QUrl(url));
    pNetworkManager->get(request);
}

void ApiManager::post(QNetworkAccessManager *pNetworkManager, ApiType apiType, int urlIndex, QByteArray data)
{
    QString url = mRequestURLs[apiType][urlIndex];

    QNetworkRequest request;

    if (apiType == ApiType::Lostark)
    {
        request.setRawHeader("accept", "application/json");
        request.setRawHeader("authorization", QString("bearer %1").arg(getApiKey()).toUtf8());
    }
    else if (apiType == ApiType::LoaManager)
    {
        QSslConfiguration conf = request.sslConfiguration();
        conf.setPeerVerifyMode(QSslSocket::VerifyNone);

        request.setSslConfiguration(conf);
    }
    else
        return;

    request.setRawHeader("Content-Type", "application/json");
    request.setUrl(QUrl(url));
    pNetworkManager->post(request, data);
}
