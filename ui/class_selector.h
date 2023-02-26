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
    void initUI();

private:
    Ui::ClassSelector *ui;

    const QJsonArray m_classes;
    QList<QPushButton*> m_buttons;

    QList<QWidget*> m_widgets;
    QList<QLayout*> m_layouts;
};

#endif // CLASS_SELECTOR_H
