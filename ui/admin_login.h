#ifndef ADMIN_LOGIN_H
#define ADMIN_LOGIN_H

#include <QWidget>

namespace Ui {
class AdminLogin;
}

class AdminLogin : public QWidget
{
    Q_OBJECT

public:
    AdminLogin();
    ~AdminLogin();

private:
    Ui::AdminLogin *ui;
};

#endif // ADMIN_LOGIN_H
