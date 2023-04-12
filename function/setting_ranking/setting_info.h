#ifndef SETTING_INFO_H
#define SETTING_INFO_H

#include "api/loamanager/loamanager_api.h"

#include <QWidget>

class QLabel;

namespace Ui {
class SettingInfo;
}

class SettingInfo : public QWidget
{
    Q_OBJECT

public:
    SettingInfo(CharacterSetting characterSetting, int rank, int count, int total);
    ~SettingInfo();

private:
    void initializeLayoutRatio(int rank, int count, int total);
    void initializeLayoutInfo(const CharacterSetting &characterSetting);
    void initializeLayoutEngrave(const QString &engrave, const QString &engraveLevel);

    QLabel *createLabel(const QString &text, int fontSize, int width, const QString &style);

private:
    Ui::SettingInfo *ui;
};

#endif // SETTING_INFO_H
