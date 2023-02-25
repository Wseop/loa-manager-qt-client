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
    void initFunction();
    void initMenuButton();
    void initAdminButton();

private:
    Ui::LoaManager *ui;

    QJsonObject m_mainSetting;

    // function
    QList<QWidget*> m_functions;
    // admin button
    class AdminLogin* m_pAdminLogin;
    QPushButton* m_pAdminButton;

    QList<QWidget*> m_widgets;
};
#endif // LOAMANAGER_H
