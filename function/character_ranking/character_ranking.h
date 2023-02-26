#ifndef CHARACTER_RANKING_H
#define CHARACTER_RANKING_H

#include <QWidget>
#include <QJsonArray>

namespace Ui {
class CharacterRanking;
}

class CharacterRanking : public QWidget
{
    Q_OBJECT

private:
    CharacterRanking();
    ~CharacterRanking();

    void initFont();
    void initClassSelector();
    void initLevelSelector();
    void initRefresh();

    void addCharacterData(int index, QString server, QString cls, QString name, double itemLevel);
    void updateUI();
    void clearRankData();

    void setEnable(bool enable);

    static void tGetRankData();

public:
    static CharacterRanking* getInstance();
    static void destroyInstance();

private:
    Ui::CharacterRanking *ui;

    static CharacterRanking* m_pInstance;

    class ClassSelector* m_pClassSelector;
    QJsonArray m_rankData;

    int m_renderCount;
    QList<class QHBoxLayout*> m_hLayouts;
    QList<class QLabel*> m_labels;
    QList<class QPushButton*> m_buttons;

signals:
    void refresh();
};

#endif // CHARACTER_RANKING_H
