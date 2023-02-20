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

    void initContents();
    void loadDropTable();

    void setAlignments();
    void initRefreshButton();
    void initContentsSelector();
    void initInputDataButton();
    void initInfoLabel();

    void initRewardAdder();
    void initChaosReward();
    void initGuardianReward();

    QJsonObject buildSearchOption(QString itemName);
    void refreshPrice();

public:
    static ContentsReward* getInstance();
    static void destroyInstance();

private:
    Ui::ContentsReward *ui;

    static ContentsReward* m_pInstance;

    const int MAX_INFO = 2;
    const int MAX_CONTENTS = 2;

    QString m_localKey;
    QStringList m_contents;
    QList<QHash<QString, QStringList>> m_dropTables;
    QHash<QString, double> m_itemPrices;

    // widgets
    class QComboBox* m_pContentsSelector;
    QList<class QLabel*> m_infoLabels;
    QList<QList<class RewardWidget*>> m_rewardWidgets;
    QList<class RewardAdder*> m_rewardAdders;
    QList<QWidget*> m_widgets;
};

#endif // CONTENTS_REWARD_H
