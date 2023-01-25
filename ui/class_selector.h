#ifndef CLASS_SELECTOR_H
#define CLASS_SELECTOR_H

#include <QWidget>
#include <QJsonArray>

namespace Ui {
class ClassSelector;
}

class ClassSelector : public QWidget
{
    Q_OBJECT

public:
    ClassSelector();
    ~ClassSelector();

    QList<class QPushButton*>& getButtons();

private:
    void loadClass();
    void addWidgets();

private:
    Ui::ClassSelector *ui;

    QList<class QHBoxLayout*> m_hLayouts;
    QList<class QLabel*> m_labels;
    QList<QPushButton*> m_buttons;

    QJsonArray m_jArrClass;
};

#endif // CLASS_SELECTOR_H
