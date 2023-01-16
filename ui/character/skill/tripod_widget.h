#ifndef TRIPOD_WIDGET_H
#define TRIPOD_WIDGET_H

#include "game_data/character/skill/skill.h"
#include <QWidget>

class QLabel;

namespace Ui {
class TripodWidget;
}

class TripodWidget : public QWidget
{
    Q_OBJECT

public:
    TripodWidget(QWidget* pParent, const QList<Tripod>& tripods);
    ~TripodWidget();

private:
    void addLabels();

private:
    Ui::TripodWidget *ui;

    const int LABEL_WIDTH = 100;
    const int LABEL_HEIGHT = 25;

    const QList<Tripod>& m_tripods;
    QList<QLabel*> m_labels;
};

#endif // TRIPOD_WIDGET_H
