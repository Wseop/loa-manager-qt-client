#ifndef ENGRAVE_WIDGET_H
#define ENGRAVE_WIDGET_H

#include <QWidget>

class Engrave;
class QLabel;
class QHBoxLayout;

namespace Ui {
class EngraveWidget;
}

class EngraveWidget : public QWidget
{
    Q_OBJECT

public:
    EngraveWidget(QWidget* pParent, const Engrave* pEngrave);
    ~EngraveWidget();

private:
    void addEngraveHLayouts();


private:
    Ui::EngraveWidget *ui;

    const int LABEL_ICON_WIDTH = 50;
    const int LABEL_ICON_HEIGHT = 50;
    const int LABEL_ENGRAVE_WIDTH = 100;
    const int LABEL_ENGRAVE_HEIGHT = 25;
    const int LABEL_LEVEL_WIDTH = 50;
    const int LABEL_LEVEL_HEIGHT = 25;

    const Engrave* m_pEngrave;
    QList<QLabel*> m_labels;
    QList<QHBoxLayout*> m_hLayouts;
};

#endif // ENGRAVE_WIDGET_H
