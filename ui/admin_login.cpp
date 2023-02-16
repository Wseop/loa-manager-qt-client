#include "admin_login.h"
#include "ui_admin_login.h"
#include "ui/font_manager.h"

extern QString g_adminKey;

AdminLogin::AdminLogin() :
    ui(new Ui::AdminLogin)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icon/Home.ico"));
    this->setWindowTitle("관리자 로그인");
    ui->lbInputKey->setFont(FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, 10));
    ui->leInputKey->setFont(FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, 10));

    connect(ui->leInputKey, &QLineEdit::returnPressed, this, [&](){
        g_adminKey = ui->leInputKey->text();
        this->close();
    });
}

AdminLogin::~AdminLogin()
{
    delete ui;
}
