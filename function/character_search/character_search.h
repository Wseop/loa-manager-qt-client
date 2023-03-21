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
    void initializeCharacterTab();
    void initializeParser();

    void searchCharacter(const QString &characterName);
    void renderCharacter(Character *pCharacter);

public:
    void start() override;

    void updateParseStatus(uint8_t bit, Character *pCharacter);

public:
    static CharacterSearch *getInstance();
    static void destroyInstance();

private:
    Ui::CharacterSearch *ui;

    const uint8_t STATUS_PARSE_FINISHED = 0x7F;

    QList<std::function<void(QJsonDocument, Character *, uint8_t)>> mParsers;
    uint8_t mParseStatus;

    QHash<QString, Character *> mCharacters;
    QHash<QString, class CharacterInfo *> mCharacterInfos;

    class QLineEdit *mpLineEditCharacterName;
    class QPushButton *mpSearchButton;
    QList<QWidget *> mWidgets;
    QList<QLayout *> mLayouts;

private:
    static CharacterSearch *mpInstance;

signals:
    void parseFinished(Character *pCharacter);
    void searchRequested(const QString &characterName);
};

#endif // CHARACTER_SEARCH_H
