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
    void clearCharacterWidget();

public:
    static CharacterSearch* getInstance();
    static void destroyInstance();

    void changeCharacter(QString characterName);

private:
    Ui::CharacterSearch *ui;

    static CharacterSearch* m_pInstance;

    class Character* m_pCharacter;
    class CharacterWidget* m_pCharacterWidget;

signals:
    void updateCharacter();
};

#endif // CHARACTER_SEARCH_H
