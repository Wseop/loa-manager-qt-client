#include "document_builder.h"

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
