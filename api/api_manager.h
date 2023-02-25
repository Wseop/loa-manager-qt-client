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

public:
    static ApiManager* getInstance();
    static void destroyInstance();

    void get(QNetworkAccessManager* pNetworkManager, LostarkApi api, QString param);
    void post(QNetworkAccessManager* pNetworkManager, LostarkApi api, QByteArray data);

private:
    static ApiManager* m_pInstance;

    QStringList m_apiKeys;
    int m_keyIndex;
    QStringList m_requestUrls;
};

#endif // APIMANAGER_H
