#include "admin_login.h"
#include "ui_admin_login.h"
#include "ui/font_manager.h"
#include "api/api_manager.h"
#include "api/loamanager/loamanager_api.h"

#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>

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

    connect(ui->leInputKey, &QLineEdit::returnPressed, this, [&]()
    {
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
    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();

    connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&](QNetworkReply *pReply)
    {
        QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());

        if (response.isNull())
            return;

        mAdminKey = response.object().find("key")->toString();
    });
    connect(pNetworkManager,&QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    ApiManager::getInstance()->get(pNetworkManager, ApiType::LoaManager, static_cast<int>(LoamanagerApi::Admin), "0");
}
