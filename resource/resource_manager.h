#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QObject>
#include <QHash>

class ResourceManager : public QObject
{
    Q_OBJECT

private:
    ResourceManager();
    ~ResourceManager();

    void initializeIconPath();
    void initializeEquipSetName();

public:
    static ResourceManager *getInstance();
    static void destroyInstance();

public:
    QJsonObject loadJson(QString fileName);
    QString iconPath(QString item);
    QStringList equipSetNames(QString itemSet);

private:
    static ResourceManager *mpInstance;

private:
    QHash<QString, QString> mIconPath;
    QHash<QString, QStringList> mEquipSetNames;
};

#endif // RESOURCEMANAGER_H
