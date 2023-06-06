#ifndef REWARD_H
#define REWARD_H

#include <QString>
#include <QList>

struct RewardItem {
    QString item;
    int count = -1;
};

struct Reward {
    QString level;
    int cost = -1;
    QList<RewardItem> items;
};

struct ContentReward {
    QString content;
    QList<Reward> rewards;
};

#endif // REWARD_H
