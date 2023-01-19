#ifndef ENGRAVE_WIDGET_H
#define ENGRAVE_WIDGET_H

#include <QWidget>

namespace Ui {
class EngraveWidget;
}

class EngraveWidget : public QWidget
{
    Q_OBJECT

public:
    EngraveWidget(QWidget* pParent, const class Engrave* pEngrave);
    ~EngraveWidget();

private:
    void setFonts();
    void addEngraveLayouts();

private:
    Ui::EngraveWidget *ui;

    const int ICON_WIDTH = 50;
    const int ICON_HEIGHT = 50;
    const int LABEL_WIDTH = 50;
    const int LABEL_HEIGHT = 25;

    const Engrave* m_pEngrave;
    QList<class QLabel*> m_labels;
    QList<class QVBoxLayout*> m_vLayouts;
};

#endif // ENGRAVE_WIDGET_H
