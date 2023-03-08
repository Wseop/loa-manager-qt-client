#include "document_builder.h"

#include <QJsonObject>

bsoncxx::builder::stream::document DocumentBuilder::buildDocumentContentsReward(int count, QString level, QStringList items, QList<int> itemCounts)
{
    bsoncxx::builder::stream::document doc{};

    doc << "Count" << count;
    doc << "Level" << level.toStdString();
    for (int i = 0; i < items.size(); i++)
    {
        doc << items[i].toStdString() << itemCounts[i];
    }

    return doc;
}
