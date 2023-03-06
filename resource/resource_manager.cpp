#include "resource_manager.h"

#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>

ResourceManager *ResourceManager::m_pInstance = nullptr;

ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{

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
