#include "requestbody_builder.h"

#include <QJsonDocument>
#include <QJsonObject>

QByteArray RequestBodyBuilder::buildRewardBody(QString level, int count, QStringList items, QList<int> itemCounts)
{
    QJsonObject rewardBody;

    rewardBody.insert("level", level);
    rewardBody.insert("count", count);

    for (int i = 0; i < items.size(); i++)
    {
        const QString &item = items[i];
        int itemCount = itemCounts[i];

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

        rewardBody.insert(key, itemCount);
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
