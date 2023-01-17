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
    void addProfileWidget();
    void addEquipWidgets();
    void addAccessoryWidgets();

private:
    Ui::CharacterWidget *ui;

    const Character* m_pCharacter;
    QList<class QLabel*> m_labels;
    QList<class EquipWidget*> m_equipWidgets;
    QList<class AccessoryWidget*> m_accessoryWidgets;
};

#endif // CHARACTER_WIDGET_H
