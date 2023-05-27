#include "engrave_manager.h"
#include "api/api_manager.h"
#include "api/loamanager/loamanager_api.h"

#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>

EngraveManager *EngraveManager::mpEngrave = nullptr;

EngraveManager::EngraveManager()
{
    initalizeEngraveList();
}

EngraveManager::~EngraveManager()
{

}

void EngraveManager::initalizeEngraveList()
{
    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();

    connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&](QNetworkReply *pReply)
    {
        QJsonArray engraves = QJsonDocument::fromJson(pReply->readAll()).array();

        for (const QJsonValue &value : engraves)
        {
            const QJsonObject &engrave = value.toObject();

            const int &code = engrave.find("code")->toInt();
            const QString &engraveName = engrave.find("engraveName")->toString();
            const QString &className = engrave.find("className")->toString();
            const bool &isPenalty = engrave.find("isPenalty")->toBool();

            // engrave, code mapping
            mEngraveMap[code] = engraveName;

            // 각인 종류별로 리스트에 추가
            if (isPenalty)
                mPenalties << engraveName;
            else if (className.isEmpty())
                mBattleEngraves << engraveName;
            else
                mClassEngraves[className] << engraveName;
        }

        qDebug() << "EngraveManager initialized";
    });
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);
    connect(pNetworkManager, &QNetworkAccessManager::finished, &mEventLoop, &QEventLoop::quit);

    ApiManager::getInstance()->getResources(pNetworkManager, Resources::Engrave, "");

    mEventLoop.exec();
}

EngraveManager *EngraveManager::getInstance()
{
    if (mpEngrave == nullptr)
        mpEngrave = new EngraveManager();

    return mpEngrave;
}

void EngraveManager::destroyInstance()
{
    if (mpEngrave == nullptr)
        return;

    delete mpEngrave;
    mpEngrave = nullptr;
}

QStringList EngraveManager::getEngraves() const
{
    return mEngraveMap.values();
}

QStringList EngraveManager::getBattleEngraves() const
{
    return mBattleEngraves;
}

QStringList EngraveManager::getClassEngraves(const QString &className) const
{
    if (!mClassEngraves.contains(className))
        return {};

    return mClassEngraves.value(className);
}

QStringList EngraveManager::getClassEngraves() const
{
    QStringList classEngraves;

    for (auto iter = mClassEngraves.begin(); iter != mClassEngraves.end(); iter++)
    {
        classEngraves << iter.value();
    }

    return classEngraves;
}

QStringList EngraveManager::getPenalties() const
{
    return mPenalties;
}

const QString EngraveManager::iconPath(const QString &engrave) const
{
    return QString(":/image/engrave/%1.png").arg(mEngraveMap.key(engrave));
}

bool EngraveManager::isClassEngrave(const QString &engrave)
{
    if (mBattleEngraves.contains(engrave) ||
        mPenalties.contains(engrave)) {
        return false;
    } else {
        return true;
    }
}

bool EngraveManager::isPenalty(const QString &engrave)
{
    return mPenalties.contains(engrave);
}

int EngraveManager::getEngraveCode(const QString &engrave)
{
    return mEngraveMap.key(engrave);
}

const QString EngraveManager::getEngraveName(const int &code) const
{
    return mEngraveMap.value(code);
}
