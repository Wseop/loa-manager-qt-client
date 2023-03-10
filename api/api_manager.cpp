#include "api_manager.h"
#include "db/db_manager.h"
#include "resource/resource_manager.h"

#include <QFile>
#include <QJsonDocument>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

ApiManager *ApiManager::m_pInstance = nullptr;

ApiManager::ApiManager() :
    m_keyIndex(0),
    m_requestUrls(static_cast<int>(LostarkApi::Size))
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
        m_apiKeys << key.toObject().find("Key")->toString();
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

        m_requestUrls[index] = requestUrl;
    }
}

const QString &ApiManager::getApiKey()
{
    if (m_keyIndex >= m_apiKeys.size())
        m_keyIndex = 0;
    return m_apiKeys[m_keyIndex++];
}

ApiManager *ApiManager::getInstance()
{
    if (m_pInstance == nullptr)
        m_pInstance = new ApiManager();
    return m_pInstance;
}

void ApiManager::destroyInstance()
{
    if (m_pInstance == nullptr)
        return;
    delete m_pInstance;
    m_pInstance = nullptr;
}

void ApiManager::get(QNetworkAccessManager *pNetworkManager, LostarkApi api, QString param)
{
    QString url = m_requestUrls[static_cast<int>(api)].arg(param);
    QNetworkRequest request;
    request.setRawHeader("accept", "application/json");
    request.setRawHeader("authorization", QString("bearer %1").arg(getApiKey()).toUtf8());
    request.setUrl(QUrl(url));
    pNetworkManager->get(request);
}

void ApiManager::post(QNetworkAccessManager *pNetworkManager, LostarkApi api, QByteArray data)
{
    QString url = m_requestUrls[static_cast<int>(api)];
    QNetworkRequest request;
    request.setRawHeader("accept", "application/json");
    request.setRawHeader("authorization", QString("bearer %1").arg(getApiKey()).toUtf8());
    request.setRawHeader("Content-Type", "application/json");
    request.setUrl(QUrl(url));
    pNetworkManager->post(request, data);
}
