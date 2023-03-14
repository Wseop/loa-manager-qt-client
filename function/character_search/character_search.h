#ifndef CHARACTER_SEARCH_H
#define CHARACTER_SEARCH_H

#include "function/function_widget.h"

#include <QWidget>
#include <functional>

class Character;

namespace Ui {
class CharacterSearch;
}

class CharacterSearch : public QWidget, public FunctionWidget
{
    Q_OBJECT

private:
    CharacterSearch();
    ~CharacterSearch();

    void initializeInputUI();
    void initializeParser();

    void searchCharacter(const QString &characterName);
    void updateParseStatus(uint8_t bit, Character *pCharacter);
    void renderCharacter(Character *pCharacter);

private:
    static void parseSibling(Character *pCharacter, QJsonDocument response);
    static void parseProfile(Character *pCharacter, QJsonDocument response);
    static void parseEquipment(Character *pCharacter, QJsonDocument response);
    static void parseSkill(Character *pCharacter, QJsonDocument response);
    static void parseEngrave(Character *pCharacter, QJsonDocument response);
    static void parseCard(Character *pCharacter, QJsonDocument response);
    static void parseGem(Character *pCharacter, QJsonDocument response);

public:
    static CharacterSearch *getInstance();
    static void destroyInstance();

public:
    void start() override;

private:
    Ui::CharacterSearch *ui;

    const uint8_t STATUS_PARSE_FINISHED = 0x7F;

    QList<std::function<void(Character *, QJsonDocument)>> mParsers;
    uint8_t mParseStatus;

    QHash<QString, Character *> mCharacters;

    class QLineEdit *mpLineEditCharacterName;
    class QPushButton *mpSearchButton;
    QList<QWidget *> mWidgets;
    QList<QLayout *> mLayouts;

private:
    static CharacterSearch *mpInstance;
};

#endif // CHARACTER_SEARCH_H
