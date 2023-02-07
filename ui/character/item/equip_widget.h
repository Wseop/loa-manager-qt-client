#ifndef EQUIP_WIDGET_H
#define EQUIP_WIDGET_H

#include <QWidget>

class Equip;
class QLabel;
class QProgressBar;
class QNetworkAccessManager;

namespace Ui {
class EquipWidget;
}

class EquipWidget : public QWidget
{
    Q_OBJECT

public:
    EquipWidget(QWidget* pParent, const Equip* pEquip);
    ~EquipWidget();

private:
    void addIcon();
    void addQualityBar();
    void addLabels();
    void setFonts();

private:
    Ui::EquipWidget *ui;

    const int QUALITYBAR_WIDTH = 50;
    const int QUALITYBAR_HEIGHT = 20;
    const int LABEL_WIDTH = 75;
    const int LABEL_HEIGHT = 25;

    const Equip* m_pEquip;
    QList<QLabel*> m_labels;
    QProgressBar* m_pQualityBar;
    QNetworkAccessManager* m_pNetworkManager;
};

#endif // EQUIP_WIDGET_H
