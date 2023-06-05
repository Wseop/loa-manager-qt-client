#ifndef CHARACTER_SEARCH_H
#define CHARACTER_SEARCH_H

#include "function/function_widget.h"
#include "game/character/profile.h"
#include "game/item/equipment.h"
#include "game/skill/skill.h"
#include "game/item/gem.h"
#include "game/character/collectible.h"

#include <QWidget>

class Character;
class CharacterInfo;
class Weapon;
class Armor;
class Engrave;

class QLineEdit;
class QPushButton;

namespace Ui {
class CharacterSearch;
}

class CharacterSearch : public QWidget, public FunctionWidget
{
    Q_OBJECT

private:
    CharacterSearch();
    ~CharacterSearch();

    void initializeSignal();
    void initializeInputLayout();
    void initializeCharacterTab();

    void searchCharacter(const QString &characterName);
    void searchCharacterSibling(const QString &characterName, Character *pCharacter);

    Profile parseProfile(const QJsonObject &object, bool isSibling);
    Equipment parseEquipment(const QJsonObject &object);
    Skill parseSkill(const QString &className, const QJsonObject &object);
    Gem parseGem(const QJsonObject &object);
    QPair<QString, int> parseEngrave(const QJsonObject &object);
    QPair<QString, int> parseCard(const QJsonObject &object);
    Collectible parseCollectible(const QJsonObject &object);

    void addCharacter(Character *pCharacter);

public:
    void refresh() override;

    void updateParseStatus(uint8_t bit, Character *pCharacter);

public:
    static CharacterSearch *getInstance();
    static void destroyInstance();

private:
    Ui::CharacterSearch *ui;

    QHash<QString, Character *> mCharacters;
    QHash<QString, CharacterInfo *> mCharacterInfos;

    QLineEdit *mpLineEditCharacterName;
    QPushButton *mpSearchButton;

private:
    static CharacterSearch *mpInstance;

signals:
    void searchRequested(const QString &characterName);
};

#endif // CHARACTER_SEARCH_H
