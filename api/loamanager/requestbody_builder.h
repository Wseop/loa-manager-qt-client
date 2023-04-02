#ifndef REQUESTBODYBUILDER_H
#define REQUESTBODYBUILDER_H

#include "api/loamanager/loamanager_api.h"

class RequestBodyBuilder
{
public:
    static QByteArray buildRewardBody(const Reward &reward);
    static QByteArray buildCharacterSettingBody(const CharacterSetting &characterSetting);
};

#endif // REQUESTBODYBUILDER_H
