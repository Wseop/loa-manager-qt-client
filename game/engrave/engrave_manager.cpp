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
    connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&](QNetworkReply *pReply){
        QJsonArray engraves = QJsonDocument::fromJson(pReply->readAll()).array();

        for (const QJsonValue &value : engraves) {
            const QJsonObject &engrave = value.toObject();

            const int &code = engrave.find("code")->toInt();
            const QString &text = engrave.find("engraveName")->toString();
            const QString &cls = engrave.find("className")->toString();
            const bool &isPenalty = engrave.find("isPenalty")->toBool();

            // 각인명 <-> 각인코드 맵 초기화
            mEngraveToCode[text] = code;
            mCodeToEngrave[code] = text;

            // 각인 종류별로 리스트에 추가
            if (isPenalty)
                mPenalties << text;
            else if (cls.isEmpty())
                mBattleEngraves << text;
            else
                mClassEngraves[cls] << text;
        }
    });
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);
    connect(pNetworkManager, &QNetworkAccessManager::finished, &mEventLoop, &QEventLoop::quit);

    ApiManager::getInstance()->get(pNetworkManager,
                                   ApiType::LoaManager,
                                   static_cast<int>(LoamanagerApi::GetResource),
                                   {"engrave", ""},
                                   "");
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
    return mEngraveToCode.keys();
}

QStringList EngraveManager::getBattleEngraves() const
{
    return mBattleEngraves;
}

QStringList EngraveManager::getClassEngraves(const QString &characterClass) const
{
    if (!mClassEngraves.contains(characterClass))
        return QStringList(0);

    return mClassEngraves[characterClass];
}

QStringList EngraveManager::getAllClassEngraves() const
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
    QString iconPath = ":/image/engrave/%1.png";
    return iconPath.arg(mEngraveToCode[engrave]);
}

bool EngraveManager::isClassEngrave(const QString &engrave)
{
    for (auto iter = mClassEngraves.begin(); iter != mClassEngraves.end(); iter++)
    {
        if (iter.value().contains(engrave))
            return true;
    }

    return false;
}

bool EngraveManager::isPenalty(const QString &engrave)
{
    return mPenalties.contains(engrave);
}

int EngraveManager::getEngraveCode(const QString &engrave)
{
    return mEngraveToCode[engrave];
}

const QString EngraveManager::getEngraveByCode(const int &code) const
{
    return mCodeToEngrave[code];
}
