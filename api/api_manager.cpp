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
    mLostarkKeyIndex(0)
{
    const QJsonObject resource = ResourceManager::getInstance()->loadJson("api");

    initializeRequestUrl(resource);
    initializeApiKey(resource);
}

ApiManager::~ApiManager()
{

}

void ApiManager::initializeRequestUrl(const QJsonObject &resource)
{
    const QStringList &keys = resource.find("List")->toVariant().toStringList();

    for (int i = 0; i < keys.size(); i++)
    {
        const QJsonArray &apis = resource.find(keys[i])->toArray();

        for (const QJsonValue &value : apis)
        {
            mRequestURLs[static_cast<ApiType>(i)] << value.toObject().find("RequestURL")->toString();
        }
    }
}

void ApiManager::initializeApiKey(const QJsonObject &resource)
{
    // LoaManager Api Key 초기화
    mLoaManagerApiKey = resource.find("LoaManagerApiKey")->toString();

    // Lostark Api Key 초기화
    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();

    connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&](QNetworkReply *pReply)
    {
        const QJsonArray &keys = QJsonDocument::fromJson(pReply->readAll()).array();

        for (const QJsonValue &value : keys)
        {
            mLostarkApiKeys << value.toObject().find("key")->toString();
        }
    });
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    get(pNetworkManager, ApiType::LoaManager, static_cast<int>(LoamanagerApi::ApiKey), "", "");
}

const QString &ApiManager::getLostarkApiKey()
{
    if (mLostarkKeyIndex >= mLostarkApiKeys.size())
        mLostarkKeyIndex = 0;

    return mLostarkApiKeys[mLostarkKeyIndex++];
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
        request.setRawHeader("authorization", QString("bearer %1").arg(getLostarkApiKey()).toUtf8());
    }
    else if (apiType == ApiType::LoaManager)
    {
        QSslConfiguration conf = request.sslConfiguration();
        conf.setPeerVerifyMode(QSslSocket::VerifyNone);
        request.setSslConfiguration(conf);

        request.setRawHeader("ApiKey", mLoaManagerApiKey.toUtf8());
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
        request.setRawHeader("authorization", QString("bearer %1").arg(getLostarkApiKey()).toUtf8());
    }
    else if (apiType == ApiType::LoaManager)
    {
        QSslConfiguration conf = request.sslConfiguration();
        conf.setPeerVerifyMode(QSslSocket::VerifyNone);
        request.setSslConfiguration(conf);

        request.setRawHeader("ApiKey", mLoaManagerApiKey.toUtf8());
    }
    else
        return;

    request.setRawHeader("Content-Type", "application/json");
    request.setUrl(QUrl(url));

    pNetworkManager->post(request, data);
}
