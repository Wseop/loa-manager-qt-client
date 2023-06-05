#ifndef STATISTIC_DAILY_H
#define STATISTIC_DAILY_H

#include "function/function_widget.h"

#include <QWidget>
#include <QEventLoop>

class QGroupBox;
class QVBoxLayout;
class QHBoxLayout;

namespace Ui {
class StatisticDaily;
}

class StatisticDaily : public QWidget, public FunctionWidget
{
    Q_OBJECT

private:
    StatisticDaily();
    ~StatisticDaily();

    void initializeContentsData();
    void initializeItemKeyMap();

    void initializeInfoLayout();
    QLabel *createInfoLabel();

    void initializeInputLayout();
    QGroupBox *createItemSelectorGroup();
    QVBoxLayout *createItemSelector(const QString &itemName, const QStringList &keys);

    void initializeResultLayout();
    QVBoxLayout *createContentLayout(const QString &content);
    QLabel *createContentLabel(const QString &content);
    QHBoxLayout *createLevelLayout(const QString &level);
    QVBoxLayout *createItemLayout(const QString &level, const QString &itemName);

    void loadStatisticData(const QString &content);
    void updateItemCount(const QJsonObject &data);
    void updateItemPrice(const QString &level);

public:
    void refresh() override;

public:
    static StatisticDaily *getInstance();
    static void destroyInstance();

private:
    Ui::StatisticDaily *ui;

    // contents data
    QStringList mContents;
    QHash<QString, QStringList> mContentLevels;
    QHash<QString, QStringList> mLevelItemsMap;
    QHash<QString, QString> mItemKeyMap;

    // item selector (input)
    QHash<QString, QCheckBox*> mItemSelectorMap;

    // item count
    QHash<QString, QHash<QString, double>> mItemCountMap;
    QHash<QString, QHash<QString, QLabel*>> mItemCountLabelMap;

private:
    static StatisticDaily *pInstance;
};

#endif // STATISTIC_DAILY_H
