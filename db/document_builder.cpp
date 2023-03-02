#include "document_builder.h"

#include <QJsonObject>

bsoncxx::builder::stream::document DocumentBuilder::buildDocumentContentsReward(QString level, QStringList items, QList<int> itemCounts, QString remark)
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
