#ifndef REWARDCHAOS_H
#define REWARDCHAOS_H

#include <QObject>
#include <QJsonObject>

class RewardChaos : public QObject
{
    Q_OBJECT

public:
    RewardChaos();

    void setLevel(QString level);
    void setCount(int count);
    void setSilling(int silling);
    void setShard(int shard);
    void setDestructionStone(int destructionStone);
    void setProtectionStone(int protectionStone);
    void setLeapStone(int leapStone);
    void setGem(int gem);

    QByteArray data();

private:
    QJsonObject mObject;
};

#endif // REWARDCHAOS_H
