#ifndef STATISTIC_ARMORY_H
#define STATISTIC_ARMORY_H

#include "function/function_widget.h"
#include "api/statistics/statistics.h"
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

    // class engrave layout
    void initializeClassEngraveLayout();
    QLabel *createClassEngraveLabel();

    // result layout
    void initializeResultLayout();
    QLabel *createCategoryLabel(const QString &category);
    QHBoxLayout *createCategoryLayout(const QString &category);

    // handle datas
    void searchStatistic();
    void parseStatistic(Statistics category, QNetworkReply *pReply);
    void parseEngraveStatistic(QNetworkReply *pReply);

    // ui
    void clearResult();
    void setSettingCount(const QStringList &classEngraves, Statistics category, SettingsArmory settingsArmory);
    void setEngraveSettingCount(const QStringList &classEngraves, int engraveLevel, SettingsArmory settingsArmory);
    void setClassEngraveCount(const QStringList &classEngraves, const QList<int> &counts, int totalCount);

    void enableInput(bool enable);

public:
    void refresh() override;

public:
    static StatisticArmory *getInstance();
    static void destoryInstance();

private:
    Ui::StatisticArmory *ui;

    QString mSelectedClassName;

    // ui
    QComboBox *pClassSelector;
    QPushButton *pSearchButton;
    QList<QLabel*> mClassEngraveLabels;
    QHash<QString, QList<QVBoxLayout*>> mCategoryLayouts;
    QList<QLabel*> mResultLabels;

private:
    static StatisticArmory *pInstance;
};

#endif // STATISTIC_ARMORY_H
