#ifndef GEM_WIDGET_H
#define GEM_WIDGET_H

#include <QWidget>

class Gem;
class QLabel;
class QNetworkAccessManager;

namespace Ui {
class GemWidget;
}

class GemWidget : public QWidget
{
    Q_OBJECT

public:
    GemWidget(QWidget* pParent, const Gem* pGem);
    ~GemWidget();

private:
    void addIcon();
    void addLabels();
    void setFonts();

private:
    Ui::GemWidget *ui;

    const int ICON_WIDTH = 50;
    const int ICON_HEIGHT = 50;
    const int LABEL_WIDTH = 125;
    const int LABEL_HEIGHT = 25;

    const Gem* m_pGem;
    QList<QLabel*> m_labels;
    QNetworkAccessManager* m_pNetworkManager;
};

#endif // GEM_WIDGET_H