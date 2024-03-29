#include "resource_manager.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

ResourceManager *ResourceManager::mpInstance = nullptr;

ResourceManager::ResourceManager()
{
    initializeIconPath();
    initializeEquipSetName();
}

ResourceManager::~ResourceManager()
{

}

void ResourceManager::initializeIconPath()
{
    mIconPath["어빌리티 스톤"] = ":/image/item/abilitystone/0.png";
    mIconPath["각인서"] = ":/image/item/book/0.png";
    mIconPath["보석"] = ":/image/item/gem/0.png";
    mIconPath["실링"] = ":/image/item/money/0.png";
    mIconPath["골드"] = ":/image/item/money/1.png";
    mIconPath["선명한 지혜의 기운"] = ":/image/item/vigor/0.png";
    mIconPath["빛나는 지혜의 기운"] = ":/image/item/vigor/1.png";

    initializeReforgeIconPath();
    initializeRuneIconPath();
    initializeCollectibleIconPath();
}

void ResourceManager::initializeReforgeIconPath()
{
    const QString reforgeIconPath = ":/image/item/reforge/%1_%2.png";
    const QJsonArray reforges = loadJson("reforge").find("Reforge")->toArray();

    for (const QJsonValue& value : reforges)
    {
        const QJsonObject& reforge = value.toObject();
        const QJsonArray& items = reforge.find("Items")->toArray();
        int id1 = reforge.find("Id")->toInt();

        for (const QJsonValue& value : items)
        {
            const QJsonObject& item = value.toObject();
            const QString& name = item.find("Name")->toString();
            int id2 = item.find("Id")->toInt();

            mIconPath[name] = reforgeIconPath.arg(id1).arg(id2);
        }
    }
}

void ResourceManager::initializeRuneIconPath()
{
    const QStringList runeNames = {
        "질풍", "철벽", "속행", "정화", "단죄", "집중",
        "심판", "광분", "압도", "풍요", "수호", "출혈"
    };
    const QString runeIconPath = ":/image/item/rune/%1.png";

    for (int i = 0; i < runeNames.size(); i++)
    {
        mIconPath[runeNames[i]] = runeIconPath.arg(i);
    }
}

void ResourceManager::initializeCollectibleIconPath()
{
    const QStringList collectibles = {
        "모코코 씨앗", "섬의 마음", "위대한 미술품", "거인의 심장", "이그네아의 징표",
        "항해 모험물", "세계수의 잎", "오르페우스의 별", "기억의 오르골"
    };
    const QString iconPath = ":/image/collectible/%1.png";

    for (int i = 0; i < collectibles.size(); i++)
    {
        mIconPath[collectibles[i]] = iconPath.arg(i);
    }
}

void ResourceManager::initializeEquipSetName()
{
    const QJsonObject equipSetName = loadJson("equip_set_name");
    const QStringList &setNames = equipSetName.keys();

    for (const QString &setName : setNames)
    {
        const QStringList &itemNames = equipSetName.find(setName)->toVariant().toStringList();

        for (const QString &itemName : itemNames)
        {
            mEquipSetNames[setName] << itemName;
        }
    }
}

ResourceManager *ResourceManager::getInstance()
{
    if (mpInstance == nullptr)
        mpInstance = new ResourceManager();

    return mpInstance;
}

void ResourceManager::destroyInstance()
{
    if (mpInstance == nullptr)
        return;

    delete mpInstance;
    mpInstance = nullptr;
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
    if (!mIconPath.contains(item))
        return "";
    else
        return mIconPath[item];
}

QStringList ResourceManager::equipSetNames(QString itemSet)
{
    if (!mEquipSetNames.contains(itemSet))
        return {};
    else
        return mEquipSetNames[itemSet];
}
