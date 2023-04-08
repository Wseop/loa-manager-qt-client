#ifndef SETTING_RANKING_H
#define SETTING_RANKING_H

#include "api/loamanager/loamanager_api.h"
#include "function/function_widget.h"

#include <QWidget>

class QComboBox;
class QPushButton;
class QLabel;

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

    QList<CharacterSetting> filterCharacterSettings(const QList<CharacterSetting> &characterSettings);
    void sortEngraveSetting(QString &engrave, QString &engraveLevel);
    QList<QList<CharacterSetting>> classifyByClassEngrave(const QList<CharacterSetting> &characterSettings);
    void classifyBySetting(const QList<QList<CharacterSetting>> &characterSettings);

    void showCharacterSettings();

public:
    void start() override;

public:
    static SettingRanking *getInstance();
    static void destroyInstance();

private:
    Ui::SettingRanking *ui;

    QList<int> mClassEngraveCodes;
    int mTotalDataCount;
    QList<int> mDataCounts;
    QList<QHash<QString, QList<CharacterSetting>>> mCharacterSettings;  // [직업각인][세팅ID] = 세팅 목록
    QList<QList<QPair<QString, int>>> mSettingCounts;  // 세팅ID에 해당하는 세팅 목록의 size

    QComboBox *mpClassSelector;
    QPushButton *mpSearchButton;

    QList<QWidget*> mOutputs;
    QList<QWidget*> mWidgets;
    QList<QLayout*> mLayouts;

private:
    static SettingRanking *mpInstance;
};

#endif // SETTING_RANKING_H
