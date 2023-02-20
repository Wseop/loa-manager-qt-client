#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QObject>

class ResourceManager : public QObject
{
    Q_OBJECT

private:
    ResourceManager();
    ~ResourceManager();

public:
    static ResourceManager* getInstance();
    static void destroyInstance();

    QJsonObject loadJson(QString fileName);

private:
    static ResourceManager* m_pInstance;
};

#endif // RESOURCEMANAGER_H
