#ifndef SETTING_INFO_H
#define SETTING_INFO_H

#include "api/loamanager/loamanager_api.h"

#include <QWidget>

class QHBoxLayout;

namespace Ui {
class SettingInfo;
}

class SettingInfo : public QWidget
{
    Q_OBJECT

public:
    SettingInfo(CharacterSetting characterSetting, int count, int total);
    ~SettingInfo();

private:
    void initializeTitleLayout(int count, int total);
    void initializeInfoLayout();

    QString extractClassEngrave(const QString &engrave, const QString &engraveLevel);

    void addTitleInfo(const QString &itemSet, const QString &classEngrave);
    void addRatioInfo(int count, int total);

    void addAbilityInfo(const QString &ability);
    void addEngraveInfo(const QString &engrave, const QString &engraveLevel);
    void addElixirInfo(const QString &elixir);

    QHBoxLayout *createGroupBox(const QString &title);

private:
    Ui::SettingInfo *ui;

    CharacterSetting mCharacterSetting;

    QList<QWidget*> mWidgets;
    QList<QLayout*> mLayouts;
};

#endif // SETTING_INFO_H
