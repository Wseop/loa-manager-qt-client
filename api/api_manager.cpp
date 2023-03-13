#include "api_manager.h"
#include "db/db_manager.h"
#include "resource/resource_manager.h"

#include <QFile>
#include <QJsonDocument>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

ApiManager *ApiManager::mpInstance = nullptr;

ApiManager::ApiManager() :
    mKeyIndex(0),
    mRequestUrls(static_cast<int>(LostarkApi::Size))
{
    loadApiKey();
    loadRequestUrl();
}

ApiManager::~ApiManager()
{

}

void ApiManager::loadApiKey()
{
    DbManager *pDbManager = DbManager::getInstance();
    bsoncxx::builder::stream::document dummyDoc{};

    pDbManager->lock();
    QJsonArray keys = pDbManager->findDocuments(Collection::ApiKey, SortOrder::None, "", dummyDoc.extract());
    pDbManager->unlock();

    for (const QJsonValue& key : keys)
    {
        mApiKeys << key.toObject().find("Key")->toString();
    }
}

void ApiManager::loadRequestUrl()
{
    const QJsonObject json = ResourceManager::getInstance()->loadJson("api");
    const QJsonArray &api = json.find("Api")->toArray();

    for (const QJsonValue& value : api)
    {
        const QJsonObject &obj = value.toObject();
        const int &index = obj.find("Index")->toInt();
        const QString &requestUrl = obj.find("RequestURL")->toString();

        mRequestUrls[index] = requestUrl;
    }
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

void ApiManager::get(QNetworkAccessManager *pNetworkManager, LostarkApi api, QString param)
{
    QString url = mRequestUrls[static_cast<int>(api)].arg(param);
    QNetworkRequest request;
    request.setRawHeader("accept", "application/json");
    request.setRawHeader("authorization", QString("bearer %1").arg(getApiKey()).toUtf8());
    request.setUrl(QUrl(url));
    pNetworkManager->get(request);
}

void ApiManager::post(QNetworkAccessManager *pNetworkManager, LostarkApi api, QByteArray data)
{
    QString url = mRequestUrls[static_cast<int>(api)];
    QNetworkRequest request;
    request.setRawHeader("accept", "application/json");
    request.setRawHeader("authorization", QString("bearer %1").arg(getApiKey()).toUtf8());
    request.setRawHeader("Content-Type", "application/json");
    request.setUrl(QUrl(url));
    pNetworkManager->post(request, data);
}
