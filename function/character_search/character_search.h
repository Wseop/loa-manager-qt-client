#ifndef CHARACTER_SEARCH_H
#define CHARACTER_SEARCH_H

#include "function/function_widget.h"
#include "game/character/ability.h"

#include <QWidget>
#include <functional>

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
    void initializeParser();

    void searchCharacter(const QString &characterName);
    void searchCharacterSibling(const QString &characterName, Character *pCharacter);

    void setEstherItemSet(Character *pCharacter);
    void addCharacter(Character *pCharacter);

    void updateCharacterSetting(Character *pCharacter);
    void updateSkillSetting(Character *pCharacter);

    // CharacterSetting
    QString extractItemSet(const Weapon *pWeapon, const QList<Armor *> &armors);
    QString extractEngrave(const Engrave *pEngrave);
    QString extractEngraveLevel(const Engrave *pEngrave);
    QString extractAbility(const QHash<Ability, int> &ability);
    QString extractElixir(const QList<Armor *> &armors);

public:
    void start() override;

    void updateParseStatus(uint8_t bit, Character *pCharacter);

public:
    static CharacterSearch *getInstance();
    static void destroyInstance();

private:
    Ui::CharacterSearch *ui;

    const uint8_t STATUS_PARSE_FINISHED = 0x7F;

    QList<std::function<void(QVariant, Character *)>> mParsers;
    uint8_t mParseStatus;

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
