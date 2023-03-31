#ifndef REQUESTBODYBUILDER_H
#define REQUESTBODYBUILDER_H

#include <QJsonObject>

class RequestBodyBuilder
{
public:
    static QJsonObject buildRewardBody(QString level, int count, QStringList items, QList<int> itemCounts);
};

#endif // REQUESTBODYBUILDER_H
