#ifndef REQUESTBODYBUILDER_H
#define REQUESTBODYBUILDER_H

#include "api/loamanager/loamanager_api.h"

class RequestBodyBuilder
{
public:
    static QByteArray buildRewardBody(const Reward &reward);
    static QByteArray buildCharacterSettingBody(const CharacterSetting &characterSetting);
    static QByteArray buildSkillSettingBody(const SkillSetting &skillSetting);
    static QByteArray buildLoginBody(const QString &userId, const QString &password);
};

#endif // REQUESTBODYBUILDER_H
