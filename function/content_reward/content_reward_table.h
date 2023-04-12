#ifndef CONTENT_REWARD_TABLE_H
#define CONTENT_REWARD_TABLE_H

#include "function/content_reward/content_reward.h"

#include <QWidget>

namespace Ui {
class ContentRewardTable;
}

class ContentRewardTable : public QWidget
{
    Q_OBJECT

public:
    ContentRewardTable(const QString &content, const QStringList &contentLevels, const QHash<QString, QStringList> &dropTable);
    ~ContentRewardTable();

    void refreshRewardData(const QHash<QString, Reward> newRewardData);
    void refreshTradablePrice(const QHash<QString, int> &newTradablePrice, const QHash<QString, class QCheckBox*> &tradableSelector);

private:
    void initializeUIContentLevel();
    void initializeUIDropTable();
    void initializeUIGold(int col);

private:
    Ui::ContentRewardTable *ui;

    QString mContent;
    QStringList mContentLevels;
    QHash<QString, QStringList> mDropTable;
    QHash<QString, Reward> mRewardData;
    QHash<QString, double> mTradablePrice;

    QHash<QString, QList<class QLabel*>> mItemCountLabels;
    QHash<QString, QLabel*> mGoldLabel;
    QHash<QString, QLabel*> mCountLabel;
};

#endif // CONTENT_REWARD_TABLE_H
