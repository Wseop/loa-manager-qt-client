#ifndef ABILITYSTONE_WIDGET_H
#define ABILITYSTONE_WIDGET_H

#include <QWidget>

class AbilityStone;
class QLabel;
class QNetworkAccessManager;

namespace Ui {
class AbilityStoneWidget;
}

class AbilityStoneWidget : public QWidget
{
    Q_OBJECT

public:
    AbilityStoneWidget(QWidget* pParent, const AbilityStone* pAbilityStone);
    ~AbilityStoneWidget();

private:
    void addIcon();
    void addLabels();

private:
    Ui::AbilityStoneWidget *ui;

    const int LABEL_WIDTH = 100;
    const int LABEL_HEIGHT = 25;

    const AbilityStone* m_pAbilityStone;
    QList<QLabel*> m_labels;
    QNetworkAccessManager* m_pNetworkManager;
};

#endif // ABILITYSTONE_WIDGET_H
