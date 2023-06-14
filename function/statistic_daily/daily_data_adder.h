#ifndef DAILY_DATA_ADDER_H
#define DAILY_DATA_ADDER_H

#include "api/statistics/reward_chaos.h"
#include "api/statistics/reward_guardian.h"

#include <QWidget>

class QGroupBox;
class QLabel;
class QLineEdit;
class QVBoxLayout;
class QPushButton;

class QIntValidator;

namespace Ui {
class DailyDataAdder;
}

class DailyDataAdder : public QWidget
{
    Q_OBJECT

public:
    DailyDataAdder(const QString &content, const QString &level, const QStringList &itemNames, const QHash<QString, QString> &itemKeyMap);
    ~DailyDataAdder();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QGroupBox *createLevelGroup(const QString &level);
    QLabel *createLevelLabel(const QString &level);

    QGroupBox *createDataCountGroup();
    QLineEdit *createDataCountField();

    QGroupBox *createItemGroup(const QStringList &itemNames, const QHash<QString, QString> &itemKeyMap);
    QVBoxLayout *createItemLayout(const QString &itemName, const QString &itemKey);
    QLabel *createItemIcon(const QString &itemName);
    QLineEdit *createItemCountField(const QString &itemName);

    QPushButton *createAddButton(const QString &content);

private:
    Ui::DailyDataAdder *ui;

    QIntValidator *pIntValidator;

    RewardChaos mRewardChaos;
    RewardGuardian mRewardGuardian;
};

#endif // DAILY_DATA_ADDER_H
