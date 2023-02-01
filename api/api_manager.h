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

    const QString& getApiKey(int index) const;

public:
    static ApiManager* getInstance();
    static void destroyInstance();

    const int MAX_API_KEY = 5;

    void get(QNetworkAccessManager* pNetworkManager, int apiKeyIndex, LostarkApi api, QString param);
    void post(QNetworkAccessManager* pNetworkManager, int apiKeyIndex, LostarkApi api, QByteArray data);

private:
    static ApiManager* m_pInstance;

    QStringList m_apiKeys;
    QStringList m_requestUrls;
};

#endif // APIMANAGER_H
