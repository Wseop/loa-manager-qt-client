#include "loamanager.h"

#include <QApplication>

#include "game_data/character/character_manager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoaManager w;
    w.show();

    CharacterManager::getInstance()->getCharacter("쿠키바닐라쉐이크");

    return a.exec();
}
