#ifndef BRACELET_WIDGET_H
#define BRACELET_WIDGET_H

#include <QWidget>

class Bracelet;
class QLabel;
class QNetworkAccessManager;

namespace Ui {
class BraceletWidget;
}

class BraceletWidget : public QWidget
{
    Q_OBJECT

public:
    BraceletWidget(QWidget* pParent, const Bracelet* pBracelet);
    ~BraceletWidget();

private:
    void addIcon();
    void addLabels();
    void setFonts();

private:
    Ui::BraceletWidget *ui;

    const int ICON_WIDTH = 50;
    const int ICON_HEIGHT = 50;
    const int LABEL_WIDTH = 200;
    const int LABEL_HEIGHT = 25;

    const Bracelet* m_pBracelet;
    QList<QLabel*> m_labels;
    QNetworkAccessManager* m_pNetworkManager;
};

#endif // BRACELET_WIDGET_H