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

    void initializeRequestUrl(const QJsonObject &resource);
    void initializeApiKey();

    const QString &getLostarkApiKey();

public:
    static ApiManager *getInstance();
    static void destroyInstance();

    void setAccessToken(const QString &token);
    QString accessToken() const;

    void get(QNetworkAccessManager *pNetworkManager, ApiType apiType, int urlIndex, const QStringList &params, const QString &query);
    void post(QNetworkAccessManager *pNetworkManager, ApiType apiType, int urlIndex, const QStringList &params, QByteArray data);

private:
    static ApiManager *mpInstance;

    QStringList mLostarkApiKeys;
    int mLostarkKeyIndex;
    QString mAccessToken;

    QHash<ApiType, QStringList> mRequestURLs;
};

#endif // APIMANAGER_H
