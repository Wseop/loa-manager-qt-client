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
    void initializeLoginButton();
    void initializeVersionInfo();

private:
    Ui::LoaManager *ui;

    QJsonObject mMainSetting;

    // function
    QList<QWidget*> mFunctions;

    // admin button
    QPushButton *mpLoginButton;

    // version
    class QLabel *mpLabelVersionInfo;

    QList<QPushButton*> mMenuButtons;
};
#endif // LOAMANAGER_H
