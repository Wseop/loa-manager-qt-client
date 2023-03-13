#include "admin_login.h"
#include "ui_admin_login.h"
#include "ui/font_manager.h"
#include "db/db_manager.h"
#include <QMessageBox>

extern bool gbAdmin;

AdminLogin::AdminLogin() :
    ui(new Ui::AdminLogin),
    mAdminKey("")
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icon/Home.ico"));
    this->setWindowTitle("관리자 로그인");
    ui->lbInputKey->setFont(FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, 10));
    ui->leInputKey->setFont(FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, 10));

    loadAdminKey();

    connect(ui->leInputKey, &QLineEdit::returnPressed, this, [&](){
        if (mAdminKey == "")
            return;

        QString input = ui->leInputKey->text();
        if (input == mAdminKey)
        {
            gbAdmin = true;
            this->close();
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("키가 일치하지 않습니다.");
            msgBox.exec();
        }
    });
}

AdminLogin::~AdminLogin()
{
    delete ui;
}

void AdminLogin::loadAdminKey()
{
    DbManager *pDbManager = DbManager::getInstance();
    bsoncxx::builder::stream::document filter{};
    filter << "Type" << 0;

    pDbManager->lock();
    QJsonObject result = pDbManager->findDocument(Collection::Admin, filter.extract());
    pDbManager->unlock();

    mAdminKey = result.find("Key")->toString();
}
