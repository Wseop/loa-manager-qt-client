#ifndef LOAMANAGER_H
#define LOAMANAGER_H

#include <QWidget>
#include <QPushButton>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
namespace Ui { class LoaManager; }
QT_END_NAMESPACE

class LoaManager : public QWidget
{
    Q_OBJECT

public:
    LoaManager();
    ~LoaManager();

private:
    void setLayoutAlignment();
    void createMenuButtons();

private:
    Ui::LoaManager *ui;

    const int MENU_BUTTON_WIDTH = 100;
    const int MENU_BUTTON_HEIGHT = 50;

    QJsonObject m_mainSetting;
    QPushButton* m_pBackButton;
    QList<QPushButton*> m_parentMenuButtons;
    QList<QPushButton*> m_childMenuButtons;
    QMap<QPushButton*, QList<QPushButton*>> m_parentToChildButtons;
};
#endif // LOAMANAGER_H
