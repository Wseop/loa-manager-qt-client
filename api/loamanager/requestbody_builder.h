#ifndef REQUESTBODYBUILDER_H
#define REQUESTBODYBUILDER_H

#include "api/loamanager/loamanager_api.h"

class RequestBodyBuilder
{
public:
    static QByteArray buildRewardBody(QString level, int count, QStringList items, QList<int> itemCounts);
    static QByteArray buildCharacterSettingBody(const CharacterSetting &characterSetting);
};

#endif // REQUESTBODYBUILDER_H
