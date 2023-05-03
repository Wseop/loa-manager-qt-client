#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    Login(class QPushButton *pLoginButton);
    ~Login();

protected:
    void closeEvent(QCloseEvent *event);

private:
    void initializeFont();

    void requestLogin();
    void processLogin(class QNetworkReply *pReply);

    bool checkInput();

private:
    Ui::Login *ui;

    QPushButton *mpLoginButton;
};

#endif // LOGIN_H
