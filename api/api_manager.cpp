#include "api_manager.h"
#include "db/db_manager.h"
#include <QFile>
#include <QJsonDocument>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

ApiManager* ApiManager::m_pInstance = nullptr;

ApiManager::ApiManager() :
    m_keyIndex(MAX_API_KEY),
    m_requestUrls(static_cast<int>(LostarkApi::Size))
{
    loadApiKey();
    loadRequestUrl();
}

ApiManager::~ApiManager()
{
    destroyInstance();
}

void ApiManager::loadApiKey()
{
    DbManager* pDbManager = DbManager::getInstance();
    bsoncxx::builder::stream::document dummyDoc{};

    pDbManager->lock();
    QJsonArray keys = pDbManager->findDocuments(Collection::ApiKey, SortOrder::None, "", dummyDoc.extract());
    pDbManager->unlock();

    for (const QJsonValue& key : keys)
    {
        const QJsonObject& keyObj = key.toObject();
        m_apiKeys << keyObj.find("Key")->toString();
    }
}

void ApiManager::loadRequestUrl()
{
    QFile file(":/json/json/api.json");
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << Q_FUNC_INFO << ": API load fail";
        return;
    }
    QJsonArray apis = QJsonDocument::fromJson(file.readAll()).array();
    file.close();

    for (const QJsonValue& api : apis)
    {
        const QJsonObject& apiObj = api.toObject();
        const int& index = apiObj.find("Index")->toInt();
        const QString& requestUrl = apiObj.find("RequestURL")->toString();

        m_requestUrls[index] = requestUrl;
    }
}

const QString& ApiManager::getApiKey()
{
    m_keyIndex++;
    if (m_keyIndex >= MAX_API_KEY)
        m_keyIndex = 0;
    return m_apiKeys[m_keyIndex];
}

ApiManager* ApiManager::getInstance()
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

void ApiManager::get(QNetworkAccessManager* pNetworkManager, LostarkApi api, QString param)
{
    const QString& apiKey = getApiKey();

    QString url = m_requestUrls[static_cast<int>(api)].arg(param);
    QNetworkRequest request;
    request.setRawHeader("accept", "application/json");
    request.setRawHeader("authorization", QString("bearer %1").arg(apiKey).toUtf8());
    request.setUrl(QUrl(url));
    pNetworkManager->get(request);
}

void ApiManager::post(QNetworkAccessManager* pNetworkManager, LostarkApi api, QByteArray data)
{
    const QString& apiKey = getApiKey();

    QString url = m_requestUrls[static_cast<int>(api)];
    QNetworkRequest request;
    request.setRawHeader("accept", "application/json");
    request.setRawHeader("authorization", QString("bearer %1").arg(apiKey).toUtf8());
    request.setRawHeader("Content-Type", "application/json");
    request.setUrl(QUrl(url));
    pNetworkManager->post(request, data);
}
