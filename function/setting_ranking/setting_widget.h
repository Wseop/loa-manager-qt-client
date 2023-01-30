#ifndef SETTING_WIDGET_H
#define SETTING_WIDGET_H

#include "function/setting_ranking/setting_ranking.h"
#include <QWidget>

namespace Ui {
class SettingWidget;
}

class SettingWidget : public QWidget
{
    Q_OBJECT

public:
    SettingWidget(QWidget* pParent, int index, const SettingCode& settingCode, QList<CharacterInfo> characterInfos, int total);
    ~SettingWidget();

private:
    void setFonts();
    void setAlignments();
    void setRatio(int index, int count, int total);
    void setAbility(const SettingCode& settingCode);
    void setSetEffect(const SettingCode& settingCode);
    void setEngrave(const SettingCode& settingCode);
    void setCharacterInfos(QList<CharacterInfo>& characterInfos);

private:
    Ui::SettingWidget *ui;

    const int LABEL_WIDTH = 100;
    const int LABEL_HEIGHT = 25;

    class CharacterListWidget* m_pCharacterListWidget;
    QList<class QLabel*> m_labels;
    QList<class QHBoxLayout*> m_hLayouts;
    QList<class QVBoxLayout*> m_vLayouts;
};

#endif // SETTING_WIDGET_H
