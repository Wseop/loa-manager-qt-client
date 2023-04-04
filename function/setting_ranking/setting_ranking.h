#ifndef SETTING_RANKING_H
#define SETTING_RANKING_H

#include "api/loamanager/loamanager_api.h"
#include "function/function_widget.h"

#include <QWidget>

class QComboBox;
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
    void initializeInfo();

    void mappingCharacterSettings(QList<CharacterSetting> characterSettings);
    void sortEngraveSetting(QString &engrave, QString &engraveLevel);

    void renderCharacterSettings();

public:
    void start() override;

public:
    static SettingRanking *getInstance();
    static void destroyInstance();

private:
    Ui::SettingRanking *ui;

    QHash<QString, QList<CharacterSetting>> mCharacterSettings;
    QList<QPair<QString, int>> mSettingCounts;
    int mNumOfCharacterSettings;

    QComboBox *mpClassSelector;
    QLabel *mpInfo;
    QList<QWidget*> mOutput;

    QList<QWidget*> mWidgets;
    QList<QLayout*> mLayouts;

private:
    static SettingRanking *mpInstance;
};

#endif // SETTING_RANKING_H
