#include "reward_chaos.h"

#include <QJsonDocument>

RewardChaos::RewardChaos()
{

}

void RewardChaos::setLevel(QString level)
{
    mObject.insert("level", level);
}

void RewardChaos::setCount(int count)
{
    mObject.insert("count", count);
}

void RewardChaos::setSilling(int silling)
{
    mObject.insert("silling", silling);
}

void RewardChaos::setShard(int shard)
{
    mObject.insert("shard", shard);
}

void RewardChaos::setDestructionStone(int destructionStone)
{
    mObject.insert("destructionStone", destructionStone);
}

void RewardChaos::setProtectionStone(int protectionStone)
{
    mObject.insert("protectionStone", protectionStone);
}

void RewardChaos::setLeapStone(int leapStone)
{
    mObject.insert("leapStone", leapStone);
}

void RewardChaos::setGem(int gem)
{
    mObject.insert("gem", gem);
}

QByteArray RewardChaos::data()
{
    return QJsonDocument(mObject).toJson();
}
