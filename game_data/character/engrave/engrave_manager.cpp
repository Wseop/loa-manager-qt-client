#include "engrave_manager.h"
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

EngraveManager* EngraveManager::m_pEngrave = nullptr;

EngraveManager::EngraveManager()
{
    QFile file(":/json/json/engrave.json");
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << Q_FUNC_INFO << ": engrave.json open fail";
        return;
    }
    QJsonArray engraves = QJsonDocument::fromJson(file.readAll()).array();
    file.close();

    for (const QJsonValue& engrave : engraves)
    {
        const QJsonObject& engraveObj = engrave.toObject();
        const QString& text = engraveObj.find("Text")->toString();
        int code = engraveObj.find("Code")->toInt();
        QString cls = engraveObj.find("Class")->toString();
        bool isPenalty = engraveObj.find("Penalty")->toBool();

        m_engraveToCode[text] = code;
        m_codeToEngrave[code] = text;
        if (isPenalty)
            m_penalties << text;
        else if (cls.isEmpty())
            m_normalEngraves << text;
        else
            m_classEngraves << text;
    }
}

EngraveManager::~EngraveManager()
{
    destroyInstance();
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

QStringList EngraveManager::getEngraves() const
{
    return m_engraveToCode.keys();
}

const QStringList& EngraveManager::getNormalEngraves() const
{
    return m_normalEngraves;
}

const QStringList& EngraveManager::getClassEngraves() const
{
    return m_classEngraves;
}

const QStringList& EngraveManager::getPenalties() const
{
    return m_penalties;
}

QString EngraveManager::iconPath(const QString& engrave)
{
    QString iconPath = ":/engrave/image/engrave/%1.png";
    return iconPath.arg(m_engraveToCode[engrave]);
}

bool EngraveManager::isClassEngrave(QString engrave)
{
    return m_classEngraves.contains(engrave);
}

int EngraveManager::getEngraveCode(QString engrave)
{
    return m_engraveToCode[engrave];
}

QString EngraveManager::getEngraveByCode(int code)
{
    return m_codeToEngrave[code];
}
