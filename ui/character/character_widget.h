#ifndef CHARACTER_WIDGET_H
#define CHARACTER_WIDGET_H

#include <QWidget>

namespace Ui {
class CharacterWidget;
}

class CharacterWidget : public QWidget
{
    Q_OBJECT

public:
    CharacterWidget(QWidget* pParent, const class Character* pCharacter);
    ~CharacterWidget();

private:
    void setFonts();
    void setStyleSheets();
    void addProfileWidget();
    void addEquipWidgets();
    void addAccessoryWidgets();
    void addAbilityStoneWidget();
    void addBraceletWidget();
    void addEngraveWidget();

private:
    Ui::CharacterWidget *ui;

    const QString COLOR_GUILD = "#1A9362";
    const QString COLOR_SERVER = "#B178EA";
    const QString COLOR_ITEMLEVEL = "#FF009B";

    const Character* m_pCharacter;

    QList<class QLabel*> m_labels;
    QList<class QHBoxLayout*> m_hLayouts;

    QList<class EquipWidget*> m_equipWidgets;
    QList<class AccessoryWidget*> m_accessoryWidgets;
    class AbilityStoneWidget* m_pAbilityStoneWidget;
    class BraceletWidget* m_pBraceletWidget;
    class EngraveWidget* m_pEngraveWidget;
};

#endif // CHARACTER_WIDGET_H
