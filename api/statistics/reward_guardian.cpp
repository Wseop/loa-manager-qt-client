#include "reward_guardian.h"

#include <QJsonDocument>

RewardGuardian::RewardGuardian()
{

}

void RewardGuardian::setLevel(QString level)
{
    mObject.insert("level", level);
}

void RewardGuardian::setCount(int count)
{
    mObject.insert("count", count);
}

void RewardGuardian::setDestructionStone(int destructionStone)
{
    mObject.insert("destructionStone", destructionStone);
}

void RewardGuardian::setProtectionStone(int protectionStone)
{
    mObject.insert("protectionStone", protectionStone);
}

void RewardGuardian::setLeapStone(int leapStone)
{
    mObject.insert("leapStone", leapStone);
}

QByteArray RewardGuardian::data()
{
    return QJsonDocument(mObject).toJson();
}
