#ifndef ACCESSORY_WIDGET_H
#define ACCESSORY_WIDGET_H

#include <QWidget>

class Accessory;
class QLabel;
class QProgressBar;
class QNetworkAccessManager;

namespace Ui {
class AccessoryWidget;
}

class AccessoryWidget : public QWidget
{
    Q_OBJECT

public:
    AccessoryWidget(QWidget* pParent, const Accessory* pAccessory);
    ~AccessoryWidget();

private:
    void addIcon();
    void addQualityBar();
    void addLabels();

private:
    Ui::AccessoryWidget *ui;

    const int QUALITYBAR_WIDTH = 50;
    const int QUALITYBAR_HEIGHT = 20;
    const int LABEL_WIDTH = 100;
    const int LABEL_HEIGHT = 25;

    const Accessory* m_pAccessory;
    QList<QLabel*> m_labels;
    QProgressBar* m_pQualityBar;
    QNetworkAccessManager* m_pNetworkManager;
};

#endif // ACCESSORY_WIDGET_H
