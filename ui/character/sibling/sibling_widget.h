#ifndef SIBLING_WIDGET_H
#define SIBLING_WIDGET_H

#include "game_data/character/character.h"
#include <QWidget>

namespace Ui {
class SiblingWidget;
}

class SiblingWidget : public QWidget
{
    Q_OBJECT

public:
    SiblingWidget(QWidget* pParent, const QList<Other>& sibling);
    ~SiblingWidget();

private:
    void setServerToSibling();
    void addServerGridLayouts();
    void addSiblingToGrid();

private:
    Ui::SiblingWidget *ui;

    const QList<Other>& m_sibling;
    QMap<QString, QList<Other>> m_serverToSibling;
    QMap<QString, class QGridLayout*> m_serverToGridLayout;
    QList<class QLabel*> m_labels;
    QList<class QPushButton*> m_buttons;
};

#endif // SIBLING_WIDGET_H
