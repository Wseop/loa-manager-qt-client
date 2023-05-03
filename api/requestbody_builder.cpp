#include "requestbody_builder.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

QByteArray RequestBodyBuilder::buildRewardBody(const Reward &reward)
{
    QJsonObject rewardBody;

    rewardBody.insert("level", reward.level);
    rewardBody.insert("count", reward.count);

    for (int i = 0; i < reward.items.size(); i++)
    {
        const QString &item = reward.items[i];

        QString key;

        if (item == "실링")
            key = "silling";
        else if (item == "명예의 파편")
            key = "shard";
        else if (item.contains("파괴"))
            key = "destruction";
        else if (item.contains("수호"))
            key = "protection";
        else if (item.contains("돌파석"))
            key = "leapStone";
        else if (item == "보석")
            key = "gem";

        rewardBody.insert(key, reward.itemCounts[i]);
    }

    return QJsonDocument(rewardBody).toJson();
}

QByteArray RequestBodyBuilder::buildCharacterSettingBody(const CharacterSetting &characterSetting)
{
    QJsonObject body;

    body.insert("characterName", characterSetting.characterName);
    body.insert("className", characterSetting.className);
    body.insert("itemLevel", characterSetting.itemLevel);
    body.insert("itemSet", characterSetting.itemSet);
    body.insert("engrave", characterSetting.engrave);
    body.insert("engraveLevel", characterSetting.engraveLevel);
    body.insert("ability", characterSetting.ability);

    if (characterSetting.elixir != "")
        body.insert("elixir", characterSetting.elixir);

    return QJsonDocument(body).toJson();
}

QByteArray RequestBodyBuilder::buildSkillSettingBody(const SkillSetting &skillSetting)
{
    QJsonObject body;

    body.insert("characterName", skillSetting.characterName);
    body.insert("className", skillSetting.className);

    // 직업각인 데이터 추가
    QJsonArray classEngraves;

    for (const QString &classEngrave : skillSetting.classEngraves)
    {
        classEngraves.append(classEngrave);
    }

    body.insert("classEngrave", classEngraves);

    // 스킬 데이터 추가
    QJsonArray skills;

    for (const SkillSetting::SkillData &skillData : skillSetting.skills)
    {
        QJsonObject skill;
        QJsonArray tripodNames;

        for (const QString &tripodName : skillData.tripodsNames)
        {
            tripodNames.append(tripodName);
        }

        skill.insert("tripodNames", tripodNames);
        skill.insert("skillName", skillData.skillName);
        skill.insert("runeName", skillData.runeName);

        skills.append(skill);
    }

    body.insert("skill", skills);

    return QJsonDocument(body).toJson();
}

QByteArray RequestBodyBuilder::buildLoginBody(const QString &userId, const QString &password)
{
    QJsonObject loginBody;

    loginBody.insert("userId", userId);
    loginBody.insert("password", password);

    return QJsonDocument(loginBody).toJson();
}
