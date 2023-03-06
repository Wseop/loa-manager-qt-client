#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "db/db_enums.h"

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/document.hpp>

#include <QObject>
#include <QMutex>
#include <QJsonObject>
#include <QJsonArray>

class DbManager : public QObject
{
    Q_OBJECT

private:
    DbManager();
    ~DbManager();

    void connect();
    mongocxx::collection getCollection(Collection collection);

public:
    static DbManager *getInstance();
    static void destroyInstance();

    void lock();
    void unlock();

    QJsonObject findDocument(Collection collection, bsoncxx::document::value filter);
    QJsonArray findDocuments(Collection collection, SortOrder order, QString orderField, bsoncxx::document::value filter);
    void insertDocument(Collection collection, bsoncxx::document::value doc, bsoncxx::document::value filter, bool bForceInsert);
    void deleteDocument(Collection collection, bsoncxx::document::value filter);

private:
    static DbManager *m_pInstance;

    QMutex m_mutex;
    mongocxx::instance m_instance{};
    mongocxx::client m_client;
    mongocxx::database m_db;
};

#endif // DBMANAGER_H
