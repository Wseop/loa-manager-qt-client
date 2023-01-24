#ifndef CHARACTER_SEARCH_H
#define CHARACTER_SEARCH_H

#include <QWidget>

namespace Ui {
class CharacterSearch;
}

class CharacterSearch : public QWidget
{
    Q_OBJECT

private:
    CharacterSearch();
    ~CharacterSearch();

    void initConnects();
    void setFonts();
    void searchCharacter();

public:
    static CharacterSearch* getInstance();
    static void destroyInstance();

    void changeCharacter(QString characterName);

private:
    Ui::CharacterSearch *ui;

    static CharacterSearch* m_pInstance;

    QMap<int, class CharacterWidget*> m_characterWidgets;

signals:
    void updateCharacter();
};

#endif // CHARACTER_SEARCH_H
