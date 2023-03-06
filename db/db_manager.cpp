#include "db_manager.h"

#include <mongocxx/uri.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/options/find.hpp>
#include <bsoncxx/json.hpp>

#include <QSettings>
#include <QJsonDocument>

using namespace bsoncxx::builder::stream;

DbManager *DbManager::m_pInstance = nullptr;

DbManager::DbManager()
{
    connect();
}

DbManager::~DbManager()
{
    destroyInstance();
}

void DbManager::connect()
{
    QSettings config("../../config.ini", QSettings::Format::IniFormat);
    QString userName = config.value("database/user_name").toString();
    QString password = config.value("database/password").toString();

    QString dbUri = QString("mongodb+srv://%1:%2@cluster0.88yln.mongodb.net/?retryWrites=true&w=majority").arg(userName, password);
    m_client = {mongocxx::uri{dbUri.toStdString()}};
    m_db = m_client["LoaManager"];
}

mongocxx::collection DbManager::getCollection(Collection collection)
{
    switch (collection)
    {
    case Collection::Admin:
        return m_db["Admin"];
    case Collection::ApiKey:
        return m_db["ApiKey"];
    case Collection::Character:
        return m_db["Character"];
    case Collection::Setting:
        return m_db["Setting"];
    case Collection::Reward_Chaos:
        return m_db["Reward_Chaos"];
    case Collection::Reward_Guardian:
        return m_db["Reward_Guardian"];
    }
}

DbManager *DbManager::getInstance()
{
    if (m_pInstance == nullptr)
        m_pInstance = new DbManager();
    return m_pInstance;
}

void DbManager::destroyInstance()
{
    if (m_pInstance == nullptr)
        return;
    delete m_pInstance;
    m_pInstance = nullptr;
}

void DbManager::lock()
{
    m_mutex.lock();
}

void DbManager::unlock()
{
    m_mutex.unlock();
}

QJsonObject DbManager::findDocument(Collection collection, bsoncxx::document::value filter)
{
    QJsonObject result{};

    auto doc = getCollection(collection).find_one(filter.view());
    if (doc)
        result = QJsonDocument::fromJson(bsoncxx::to_json(*doc).data()).object();

    return result;
}

QJsonArray DbManager::findDocuments(Collection collection, SortOrder order, QString orderField, bsoncxx::document::value filter)
{
    QJsonArray result{};

    // set sort option
    mongocxx::options::find options{};
    document orderDoc{};
    if (order == SortOrder::Asc)
        orderDoc << orderField.toStdString() << 1;
    else if (order == SortOrder::Desc)
        orderDoc << orderField.toStdString() << -1;
    options.sort(orderDoc.view());

    mongocxx::cursor cursor = getCollection(collection).find(filter.view(), options);
    for (auto doc : cursor)
    {
        result.append(QJsonDocument::fromJson(bsoncxx::to_json(doc).data()).object());
    }

    return result;
}

void DbManager::insertDocument(Collection collection, bsoncxx::document::value doc, bsoncxx::document::value filter, bool bForceInsert)
{
    auto data = getCollection(collection).find_one(filter.view());
    // already exist, update
    if (!bForceInsert && data)
        getCollection(collection).update_one(filter.view(), document{} << "$set" << doc << finalize);
    // if new data, insert
    else
        getCollection(collection).insert_one(doc.view());
}

void DbManager::deleteDocument(Collection collection, bsoncxx::document::value filter)
{
    getCollection(collection).delete_one(filter.view());
}
