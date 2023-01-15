#ifndef CHARACTERMANAGER_H
#define CHARACTERMANAGER_H

#include <QObject>
#include <QMap>
#include <QRegularExpression>
#include <QMutex>

class Character;
class QNetworkAccessManager;
class QNetworkReply;

class CharacterManager : public QObject
{
    Q_OBJECT

private:
    CharacterManager();
    ~CharacterManager();

    void initNetworkManagers();

    void requestCharacterInfo();
    void handleSibling(QNetworkReply* pReply);
    void handleProfile(QNetworkReply* pReply);
    void handleEquipment(QNetworkReply* pReply);
    void handleSkill(QNetworkReply* pReply);
    void handleEngrave(QNetworkReply* pReply);
    void handleCard(QNetworkReply* pReply);
    void handleGem(QNetworkReply* pReply);
    void handleCollectible(QNetworkReply* pReply);
    void setHandlerStatus(uint8_t bit);

public:
    static CharacterManager* getInstance();
    static void destroyInstance();

    Character* getCharacter(QString name);

private:
    static CharacterManager* m_pInstance;

    const int CHARACTER_API_COUNT = 8;
    const uint8_t HANDLER_STATUS_FINISH = 0xFF;

    QMap<QString, Character*> m_nameToCharacter;
    QList<QNetworkAccessManager*> m_networkManagers;
    QString m_requestedCharacterName;
    uint8_t m_handlerStatus;
    QRegularExpression m_regExpHtmlTag;
    QMutex m_mutex;
};

#endif // CHARACTERMANAGER_H
