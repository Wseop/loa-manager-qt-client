#ifndef COLLECTIBLE_WIDGET_H
#define COLLECTIBLE_WIDGET_H

#include <QWidget>

class Collectible;
class QLabel;

namespace Ui {
class CollectibleWidget;
}

class CollectibleWidget : public QWidget
{
    Q_OBJECT

public:
    CollectibleWidget(QWidget* pParent, const QList<Collectible*>& collectibles);
    ~CollectibleWidget();

private:
    void addCollectibleLabels();

private:
    Ui::CollectibleWidget *ui;

    const int LABEL_WIDTH = 150;
    const int LABEL_HEIGHT = 25;

    const QList<Collectible*>& m_collectibles;
    QList<QLabel*> m_labels;
};

#endif // COLLECTIBLE_WIDGET_H
