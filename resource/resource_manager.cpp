#include "resource_manager.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

ResourceManager *ResourceManager::m_pInstance = nullptr;

ResourceManager::ResourceManager()
{
    initializeIconPath();
}

ResourceManager::~ResourceManager()
{

}

void ResourceManager::initializeIconPath()
{
    m_iconPath["어빌리티 스톤"] = ":/image/item/abilitystone/0.png";
    m_iconPath["각인서"] = ":/image/item/book/0.png";
    m_iconPath["보석"] = ":/image/item/gem/0.png";
    m_iconPath["실링"] = ":/image/item/money/0.png";
    m_iconPath["골드"] = ":/image/item/money/1.png";
    m_iconPath["선명한 지혜의 기운"] = ":/image/item/vigor/0.png";
    m_iconPath["빛나는 지혜의 기운"] = ":/image/item/vigor/1.png";

    QString reforgeIconPath = ":/image/item/reforge/%1_%2.png";
    QJsonArray reforges = loadJson("reforge").find("Reforge")->toArray();
    for (const QJsonValue& value : reforges)
    {
        const QJsonObject& reforge = value.toObject();
        int id1 = reforge.find("Id")->toInt();
        const QJsonArray& items = reforge.find("Items")->toArray();

        for (const QJsonValue& value : items)
        {
            const QJsonObject& item = value.toObject();
            const QString& name = item.find("Name")->toString();
            int id2 = item.find("Id")->toInt();

            m_iconPath[name] = reforgeIconPath.arg(id1).arg(id2);
        }
    }
}

ResourceManager *ResourceManager::getInstance()
{
    if (m_pInstance == nullptr)
        m_pInstance = new ResourceManager();

    return m_pInstance;
}

void ResourceManager::destroyInstance()
{
    if (m_pInstance == nullptr)
        return;

    delete m_pInstance;
    m_pInstance = nullptr;
}

QJsonObject ResourceManager::loadJson(QString fileName)
{
    const QString filePath = ":/json/%1.json";

    QFile file(filePath.arg(fileName));
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << Q_FUNC_INFO << fileName << " Open fail";
        return QJsonObject();
    }

    QJsonObject json = QJsonDocument::fromJson(file.readAll()).object();
    file.close();

    return json;
}

QString ResourceManager::iconPath(QString item)
{
    if (!m_iconPath.contains(item))
        return "";
    else
        return m_iconPath[item];
}
