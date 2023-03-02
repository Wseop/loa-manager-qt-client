#ifndef DOCUMENTBUILDER_H
#define DOCUMENTBUILDER_H

#include <bsoncxx/builder/stream/document.hpp>

#include <QObject>

class DocumentBuilder : public QObject
{
    Q_OBJECT

public:
    static bsoncxx::builder::stream::document buildDocumentContentsReward(QString level, QStringList items, QList<int> itemCounts, QString remark);
};

#endif // DOCUMENTBUILDER_H
