#include "engrave_manager.h"
#include "resource/resource_manager.h"

#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

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
    const QJsonArray engraves = ResourceManager::getInstance()->loadJson("engrave").find("Engrave")->toArray();

    for (const QJsonValue& value : engraves)
    {
        const QJsonObject &engrave = value.toObject();

        const int &code = engrave.find("Code")->toInt();
        const QString &text = engrave.find("Text")->toString();
        const QString &cls = engrave.find("Class")->toString();
        const bool &isPenalty = engrave.find("Penalty")->toBool();

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
