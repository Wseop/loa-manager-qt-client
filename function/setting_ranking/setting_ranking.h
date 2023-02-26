#ifndef SETTING_RANKING_H
#define SETTING_RANKING_H

#include <QWidget>
#include <QJsonArray>

using SettingCode = QString;

struct CharacterInfo
{
    QString name;
    QString cls;
    double itemLevel;
};

namespace Ui {
class SettingRanking;
}

class SettingRanking : public QWidget
{
    Q_OBJECT

private:
    SettingRanking();
    ~SettingRanking();

    void initFont();
    void initClassSelector();
    void initRefresh();
    void initShowMore();

    void updateUI();
    void clear();

    static void tLoadSettingData(QString cls);

public:
    static SettingRanking* getInstance();
    static void destroyInstance();

private:
    Ui::SettingRanking *ui;

    static SettingRanking* m_pInstance;

    class ClassSelector* m_pClassSelector;
    QMultiMap<SettingCode, CharacterInfo> m_settingCodeToCharacterInfos;
    QList<QPair<SettingCode, int>> m_settingCodeCounts;
    int m_total;
    QList<class SettingWidget*> m_settingWidgets;
    int m_renderCount;

signals:
    void refresh();
};

#endif // SETTING_RANKING_H
