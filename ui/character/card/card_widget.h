#ifndef CARD_WIDGET_H
#define CARD_WIDGET_H

#include <QWidget>

class Card;
class QLabel;

namespace Ui {
class CardWidget;
}

class CardWidget : public QWidget
{
    Q_OBJECT

public:
    CardWidget(QWidget* pParent, const QList<Card*>& cards);
    ~CardWidget();

private:
    void addCardLabels();

private:
    Ui::CardWidget *ui;

    const int LABEL_WIDTH = 200;
    const int LABEL_HEIGHT = 25;

    const QList<Card*>& m_cards;
    QList<QLabel*> m_labels;
};

#endif // CARD_WIDGET_H
