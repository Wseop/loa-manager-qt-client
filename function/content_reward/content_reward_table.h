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

    void refreshRewardData(const QHash<QString, RewardData> &newRewardData);
    void refreshTradablePrice(const QHash<QString, int> &newTradablePrice, const QHash<QString, class QCheckBox*> &tradableSelector);

private:
    void initializeUIContentLevel();
    void initializeUIDropTable();
    void initializeUIGold(int col);

private:
    Ui::ContentRewardTable *ui;

    QString m_content;
    QStringList m_contentLevels;
    QList<int> m_stages;
    QHash<QString, QStringList> m_dropTable;
    QHash<QString, RewardData> m_rewardData;
    QHash<QString, double> m_tradablePrice;

    QHash<QString, QList<class QLabel*>> m_itemCountLabels;
    QHash<QString, QLabel*> m_goldLabel;
    QHash<QString, QLabel*> m_countLabel;

    QList<QWidget*> m_widgets;
    QList<QLayout*> m_layouts;
};

#endif // CONTENT_REWARD_TABLE_H
