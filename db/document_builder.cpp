#include "document_builder.h"
#include <QJsonObject>

bsoncxx::builder::stream::document DocumentBuilder::buildDocumentCharacter(QString name, QString server, Class cls, double level)
{
    bsoncxx::builder::stream::document doc{};

    doc << "Name" << name.toStdString()
        << "Server" << server.toStdString()
        << "Class" << classToStr(cls).toStdString()
        << "Level" << level;

    return doc;
}

bsoncxx::builder::stream::document DocumentBuilder::buildDocumentSetting(QString name, Class cls, double level, QString settingCode)
{
    bsoncxx::builder::stream::document doc{};

    doc << "Name" << name.toStdString()
        << "Class" << classToStr(cls).toStdString()
        << "Level" << level
        << "SettingCode" << settingCode.toStdString();

    return doc;
}

bsoncxx::builder::stream::document DocumentBuilder::buildDocumentRewardChaos(QString level, QStringList items, QList<int> itemCounts, QString remark)
{
    bsoncxx::builder::stream::document doc{};

    doc << "Level" << level.toStdString();
    for (int i = 0; i < items.size(); i++)
    {
        doc << items[i].toStdString() << itemCounts[i];
    }
    doc << "Remark" << remark.toStdString();

    return doc;
}
