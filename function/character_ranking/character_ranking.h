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

    void setAlignments();
    void setFonts();
    void initConnects();
//    void getRankingData();
    void addCharacterData(int index, QString server, QString cls, QString name, double itemLevel);
    void updateUI();
    void clearRankingData();
    void setEnable(bool enable);

    static void tGetRankingData();

public:
    static CharacterRanking* getInstance();
    static void destroyInstance();

    bool loaded();

private:
    Ui::CharacterRanking *ui;

    static CharacterRanking* m_pInstance;

    bool m_bLoaded; // always true after load of rank_data
    class ClassSelector* m_pClassSelector;
    QJsonArray m_jArrRankingData;
    QList<class QHBoxLayout*> m_hLayouts;
    QList<class QLabel*> m_labels;
    QList<class QPushButton*> m_buttons;
    int m_renderCount;

signals:
    void refresh();
};

#endif // CHARACTER_RANKING_H
