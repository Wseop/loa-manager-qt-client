#ifndef CONTENTS_REWARD_H
#define CONTENTS_REWARD_H

#include <QWidget>
#include <QHash>

namespace Ui {
class ContentsReward;
}

class ContentsReward : public QWidget
{
    Q_OBJECT

private:
    ContentsReward();
    ~ContentsReward();

    void loadDropTable();

    void initRefreshButton();
    void initContentsSelector();
    void initInsertDataButton();
    void initRewardAdder();
    void initInfoLabel();

    // 컨텐츠별 reward ui 초기화
    void initChaosReward();
    void initGuardianReward();

    void refreshPrice();

public:
    static ContentsReward* getInstance();
    static void destroyInstance();

private:
    Ui::ContentsReward *ui;

    static ContentsReward* m_pInstance;

    const int MAX_INFO = 2;

    QStringList m_contents;
    QList<QHash<QString, QStringList>> m_dropTables;
    class QComboBox* m_pContentsSelector;

    QList<QList<class RewardWidget*>> m_rewardWidgets;
    QList<class RewardAdder*> m_rewardAdders;

    QHash<QString, double> m_itemPrices;

    QList<QWidget*> m_widgets;
};

#endif // CONTENTS_REWARD_H
