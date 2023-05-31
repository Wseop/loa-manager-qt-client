#ifndef STATISTIC_ARMORY_H
#define STATISTIC_ARMORY_H

#include "function/function_widget.h"
#include "api/statistics/settings_armory.h"

#include <QWidget>

class QGroupBox;
class QComboBox;
class QPushButton;
class QHBoxLayout;
class QVBoxLayout;

class QNetworkReply;

namespace Ui {
class StatisticArmory;
}

class StatisticArmory : public QWidget, public FunctionWidget
{
    Q_OBJECT

private:
    StatisticArmory();
    ~StatisticArmory();

    // class layout
    void initializeClassLayout();
    QGroupBox *createClassSelector();
    QPushButton *createSearchButton();
    QLabel *createInfoLabel();

    // total layout
    void initializeTotalLayout();
    QLabel *createTotalLabel();

    // class engrave layout
    void initializeClassEngraveLayout();
    QLabel *createClassEngraveLabel();

    // result layout
    void initializeResultLayout();
    QLabel *createCategoryLabel(const QString &category);
    QHBoxLayout *createCategoryLayout(const QString &category);

    // handle datas
    void searchStatistic();
    void parseStatistic(QNetworkReply *pReply);

    // ui
    void clearResult();
    void setTotalCount();
    void setClassEngraveCount(const QStringList &classEngraves);
    void setAbilityCount(const QStringList &classEngraves);
    void setEngraveCount(const QStringList &classEngraves);
    void setItemSetCount(const QStringList &classEngraves);
    void setElixirCount(const QStringList &classEngraves);

    void enableInput(bool enable);

public:
    void start() override;

public:
    static StatisticArmory *getInstance();
    static void destoryInstance();

private:
    Ui::StatisticArmory *ui;

    QString mSelectedClassName;
    SettingsArmory mStatisticData;

    // ui
    QComboBox *pClassSelector;
    QPushButton *pSearchButton;
    QLabel *pTotalLabel;
    QList<QLabel*> mClassEngraveLabels;
    QHash<QString, QList<QVBoxLayout*>> mCategoryLayouts;
    QList<QLabel*> mResultLabels;

private:
    static StatisticArmory *pInstance;
};

#endif // STATISTIC_ARMORY_H
