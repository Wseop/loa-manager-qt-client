#ifndef STATISTIC_ARMORY_H
#define STATISTIC_ARMORY_H

#include "function/function_widget.h"
#include "api/statistics/statistics.h"

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
    QLabel *createInfoLabel();

    // class engrave layout
    void initializeClassEngraveLayout();
    QPushButton *createClassEngraveButton();

    // result layout
    void initializeResultLayout();
    QLabel *createCategoryLabel(const QString &category);
    QVBoxLayout *createResultLayout(const QString &category);

    // handle datas
    void searchProfile(const QString &className);
    void parseProfile(QNetworkReply *pReply);

    void searchStatistic(const QString &classEngrave);
    void parseStatistic(Statistics category, QString engraveLevel, QNetworkReply *pReply);

    // ui
    void clearResult();
    void setResult(Statistics category, const QString &engraveLevel, const QString &value, const double &ratio);
    void enableInput(bool enable);

public:
    void refresh() override;

public:
    static StatisticArmory *getInstance();
    static void destoryInstance();

private:
    Ui::StatisticArmory *ui;

    QStringList mClassEngraves;
    QStringList mParseMainKeys;
    QStringList mParseSubKeys;
    QStringList mLayoutKeys;

    int mResponseCount;

    // ui
    QComboBox *pClassSelector;
    QList<QPushButton*> mClassEngraveButtons;
    QHash<QString, QVBoxLayout*> mResultLayoutMap;
    QList<QLabel*> mResultLabels;

private:
    static StatisticArmory *pInstance;
};

#endif // STATISTIC_ARMORY_H
