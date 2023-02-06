#ifndef ENGRAVE_SELECTOR_H
#define ENGRAVE_SELECTOR_H

#include <QWidget>

namespace Ui {
class EngraveSelector;
}

class EngraveSelector : public QWidget
{
    Q_OBJECT

public:
    EngraveSelector();
    ~EngraveSelector();

    QList<class QPushButton*>& getButtons();

private:
    void setFonts();
    void setAlignments();
    void addSelectButtons();
    void addBattleEngraveButtons();
    void addClassEngraveButtons();

private:
    Ui::EngraveSelector *ui;

    const int MAX_COL = 6;
    const int BUTTON_WIDTH = 125;
    const int BUTTON_HEIGHT = 50;

    QList<class QPushButton*> m_buttons;
};

#endif // ENGRAVE_SELECTOR_H
