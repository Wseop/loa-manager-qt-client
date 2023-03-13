#ifndef APIMANAGER_H
#define APIMANAGER_H

#include "api/lostark_api.h"

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

    const QString &getApiKey();

public:
    static ApiManager *getInstance();
    static void destroyInstance();

    void get(QNetworkAccessManager *pNetworkManager, LostarkApi api, QString param);
    void post(QNetworkAccessManager *pNetworkManager, LostarkApi api, QByteArray data);

private:
    static ApiManager *mpInstance;

    QStringList mApiKeys;
    int mKeyIndex;
    QStringList mRequestUrls;
};

#endif // APIMANAGER_H
