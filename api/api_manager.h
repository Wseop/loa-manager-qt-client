#ifndef APIMANAGER_H
#define APIMANAGER_H

#include <QObject>
#include <QHash>

enum class ApiType
{
    Lostark, LoaManager
};

class QNetworkAccessManager;

class ApiManager : public QObject
{
    Q_OBJECT

private:
    ApiManager();
    ~ApiManager();

    void initializeRequestUrl();
    void initializeApiKey();

    const QString &getApiKey();

public:
    static ApiManager *getInstance();
    static void destroyInstance();

    void get(QNetworkAccessManager *pNetworkManager, ApiType apiType, int urlIndex, const QString &param, const QString &query);
    void post(QNetworkAccessManager *pNetworkManager, ApiType apiType, int urlIndex, QByteArray data);

private:
    static ApiManager *mpInstance;

    // Lostark Api key
    QStringList mApiKeys;
    int mKeyIndex;

    QHash<ApiType, QStringList> mRequestURLs;
};

#endif // APIMANAGER_H
