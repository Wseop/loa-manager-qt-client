#include "engrave_manager.h"
#include "resource/resource_manager.h"

#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

EngraveManager* EngraveManager::m_pEngrave = nullptr;

EngraveManager::EngraveManager()
{
    initalizeEngraveList();
}

EngraveManager::~EngraveManager()
{
    destroyInstance();
}

void EngraveManager::initalizeEngraveList()
{
    const QJsonArray engraves = ResourceManager::getInstance()->loadJson("engrave").find("Engrave")->toArray();

    for (const QJsonValue& value : engraves)
    {
        const QJsonObject& engrave = value.toObject();

        const int& code = engrave.find("Code")->toInt();
        const QString& text = engrave.find("Text")->toString();
        const QString& cls = engrave.find("Class")->toString();
        const bool& isPenalty = engrave.find("Penalty")->toBool();

        // 각인명 <-> 각인코드 맵 초기화
        m_engraveToCode[text] = code;
        m_codeToEngrave[code] = text;

        // 각인 종류별로 리스트에 추가
        if (isPenalty)
            m_penalties << text;
        else if (cls.isEmpty())
            m_battleEngraves << text;
        else
            m_classEngraves << text;
    }
}

EngraveManager* EngraveManager::getInstance()
{
    if (m_pEngrave == nullptr)
        m_pEngrave = new EngraveManager();

    return m_pEngrave;
}

void EngraveManager::destroyInstance()
{
    if (m_pEngrave == nullptr)
        return;

    delete m_pEngrave;
    m_pEngrave = nullptr;
}

const QStringList EngraveManager::getEngraves() const
{
    return m_engraveToCode.keys();
}

const QStringList& EngraveManager::getbattleEngraves() const
{
    return m_battleEngraves;
}

const QStringList& EngraveManager::getClassEngraves() const
{
    return m_classEngraves;
}

const QStringList& EngraveManager::getPenalties() const
{
    return m_penalties;
}

const QString EngraveManager::iconPath(const QString& engrave) const
{
    QString iconPath = ":/engrave/image/engrave/%1.png";
    return iconPath.arg(m_engraveToCode[engrave]);
}

bool EngraveManager::isClassEngrave(const QString& engrave)
{
    return m_classEngraves.contains(engrave);
}

bool EngraveManager::isPenalty(const QString& engrave)
{
    return m_penalties.contains(engrave);
}

int EngraveManager::getEngraveCode(const QString& engrave)
{
    return m_engraveToCode[engrave];
}

const QString EngraveManager::getEngraveByCode(const int& code) const
{
    return m_codeToEngrave[code];
}
