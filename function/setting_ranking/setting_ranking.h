#ifndef SETTING_RANKING_H
#define SETTING_RANKING_H

#include "function/function_widget.h"

#include <QWidget>

enum class SettingCategory
{
    ClassEngrave, ItemSet, Ability, Elixir, EngraveSingle, Engrave, Overall, size
};

class QComboBox;
class QPushButton;
class QLabel;
class QFrame;
class QVBoxLayout;

namespace Ui {
class SettingRanking;
}

class SettingRanking : public QWidget, public FunctionWidget
{
    Q_OBJECT

private:
    SettingRanking();
    ~SettingRanking();

    void initializeInputLayout();
    void initializeClassSelector();
    void initializeSearchButton();
    void initializeHelp();

    void initializeOutputLayout();
    void initializeCategoryButton();

    void sortEngraveCodes(QString &engrave, QString &engraveLevel);

    void showClassEngraveRatio();
    void showRatio(QString textColor, int index, SettingCategory category, const QList<QVBoxLayout*> &layouts, const QList<QPair<QString, int>> &classifiedData);
    void showEngraveRatio(int index, const QList<QPair<QString, int>> &engraveRatio);
    void showOverallRatio(int index, const QList<QPair<QString, int>> &overallRatio);

public:
    void start() override;

public:
    static SettingRanking *getInstance();
    static void destroyInstance();

private:
    Ui::SettingRanking *ui;

    QList<int> mClassEngraveCodes;  // 직각1, 2

    int mTotalDataCount;
    QList<int> mDataCounts;

    QComboBox *mpClassSelector;
    QPushButton *mpSearchButton;

    QList<QFrame*> mLines;
    QList<QVBoxLayout*> mClassEngraveLayouts;
    QList<QVBoxLayout*> mItemSetLayouts;
    QList<QVBoxLayout*> mAbilityLayouts;
    QList<QVBoxLayout*> mElixirLayouts;
    QList<QVBoxLayout*> mEngraveSingleLayouts;
    QList<QVBoxLayout*> mEngraveLayouts;
    QList<QVBoxLayout*> mOverallLayouts;

    QList<QList<QWidget*>> mOutputs;

private:
    static SettingRanking *mpInstance;
};

#endif // SETTING_RANKING_H
