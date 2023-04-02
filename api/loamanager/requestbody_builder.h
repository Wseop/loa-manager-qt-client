#ifndef REQUESTBODYBUILDER_H
#define REQUESTBODYBUILDER_H


class RequestBodyBuilder
{
public:
    static QByteArray buildRewardBody(QString level, int count, QStringList items, QList<int> itemCounts);
};

#endif // REQUESTBODYBUILDER_H
