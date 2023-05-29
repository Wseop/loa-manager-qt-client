#ifndef SETTING_INFO_H
#define SETTING_INFO_H

#include <QWidget>

class QLabel;

namespace Ui {
class SettingInfo;
}

class SettingInfo : public QWidget
{
    Q_OBJECT

public:
    SettingInfo();
    ~SettingInfo();

private:
    void initializeLayoutRatio(int rank, int count, int total);
    void initializeLayoutInfo();
    void initializeLayoutEngrave(const QString &engrave, const QString &engraveLevel);

    QLabel *createLabel(const QString &text, int fontSize, int width, const QString &style);

private:
    Ui::SettingInfo *ui;
};

#endif // SETTING_INFO_H
