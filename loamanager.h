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
    void initializeFunction();
    void initializeMenuButton();
    void initializeAdminButton();
    void initializeVersionInfo();

private:
    Ui::LoaManager *ui;

    QJsonObject mMainSetting;

    // function
    QList<QWidget*> mFunctions;

    // admin button
    class AdminLogin* mpAdminLogin;
    QPushButton *mpAdminButton;

    // version
    class QLabel *mpLabelVersionInfo;

    QList<QPushButton*> mMenuButtons;

    QList<QWidget*> mWidgets;
};
#endif // LOAMANAGER_H
