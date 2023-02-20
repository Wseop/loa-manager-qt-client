#ifndef APIMANAGER_H
#define APIMANAGER_H

#include "api/enum/lostark_api.h"

#include <QObject>

class QNetworkAccessManager;

class ApiManager : public QObject
{
    Q_OBJECT

private:
    ApiManager();
    ~ApiManager();

    void loadApiKey();
    void loadRequestUrl();

    const QString& getApiKey();

    void buildDefaultSearchOption(QJsonObject& searchOption, SearchType type, CategoryCode category);

public:
    static ApiManager* getInstance();
    static void destroyInstance();

    const int MAX_API_KEY = 10;

    void get(QNetworkAccessManager* pNetworkManager, LostarkApi api, QString param);
    void post(QNetworkAccessManager* pNetworkManager, LostarkApi api, QByteArray data);

    QJsonObject buildSearchOption(SearchType type, CategoryCode category, QString itemName);
    QJsonObject buildSearchOption(SearchType type, CategoryCode category, QList<int> firstOptionCodes, QList<int> secondOptionCodes);

private:
    static ApiManager* m_pInstance;

    QStringList m_apiKeys;
    int m_keyIndex;
    QStringList m_requestUrls;
};

#endif // APIMANAGER_H
