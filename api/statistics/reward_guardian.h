#ifndef REWARDGUARDIAN_H
#define REWARDGUARDIAN_H

#include <QObject>
#include <QJsonObject>

class RewardGuardian : public QObject
{
    Q_OBJECT

public:
    RewardGuardian();

    void setLevel(QString level);
    void setCount(int count);
    void setDestructionStone(int destructionStone);
    void setProtectionStone(int protectionStone);
    void setLeapStone(int leapStone);

    QByteArray data();

private:
    QJsonObject mObject;
};

#endif // REWARDGUARDIAN_H
