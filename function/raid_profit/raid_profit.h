#ifndef RAID_PROFIT_H
#define RAID_PROFIT_H

#include "function/function_widget.h"

#include <QWidget>

class QGroupBox;
class QVBoxLayout;

class RaidProfitWidget;

namespace Ui {
class RaidProfit;
}

class RaidProfit : public QWidget, public FunctionWidget
{
    Q_OBJECT

private:
    RaidProfit();
    ~RaidProfit();

    void initializeContent();
    void initializeItem();

    void loadResource();

    void initializeInputLayout();
    QGroupBox *createContentSelectorGroup();
    QGroupBox *createItemSelectorGroup();
    QVBoxLayout *createItemSelector(const QString &itemName);

    void initializeOutputLayout();
    void showSelectedContent(const QString &content);

public:
    void refresh() override;

private:
    Ui::RaidProfit *ui;

    QStringList mContents;
    QHash<QString, bool> mSelectedItems;

    QHash<QString, RaidProfitWidget*> mRaidProfitWidgetMap;

public:
    static RaidProfit *getInstance();
    static void destroyInstance();

private:
    static RaidProfit *pInstance;
};

#endif // RAID_PROFIT_H
