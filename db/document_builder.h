#ifndef DOCUMENTBUILDER_H
#define DOCUMENTBUILDER_H

#include "game_data/character/profile/enum/class.h"
#include <bsoncxx/builder/stream/document.hpp>
#include <QObject>

class DocumentBuilder : public QObject
{
    Q_OBJECT

public:
    static bsoncxx::builder::stream::document buildDocumentCharacter(QString name, QString server, Class cls, double level);
    static bsoncxx::builder::stream::document buildDocumentSetting(QString name, Class cls, double level, QString settingCode);
    static bsoncxx::builder::stream::document buildDocumentRewardChaos(QString level, QStringList items, QList<int> itemCounts, QString remark);
};

#endif // DOCUMENTBUILDER_H
