#include "login.h"
#include "ui_login.h"
#include "ui/font_manager.h"
#include "api/api_manager.h"
#include "api/loamanager/loamanager_api.h"
#include "api/requestbody_builder.h"

#include <QCloseEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

Login::Login(QPushButton *pLoginButton) :
    ui(new Ui::Login),
    mpLoginButton(pLoginButton)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":/Home.ico"));
    this->setWindowTitle("로그인");

    connect(ui->pbLogin, &QPushButton::released, this, &Login::requestLogin);
    connect(ui->leId, &QLineEdit::returnPressed, this, &Login::requestLogin);
    connect(ui->lePassword, &QLineEdit::returnPressed, this, &Login::requestLogin);

    initializeFont();
}

Login::~Login()
{
    delete ui;
}

void Login::closeEvent(QCloseEvent *event)
{
    event->accept();
    delete this;
}

void Login::initializeFont()
{
    QFont font = FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, 10);

    ui->lbId->setFont(font);
    ui->leId->setFont(font);
    ui->lbPassword->setFont(font);
    ui->lePassword->setFont(font);
    ui->pbLogin->setFont(font);
}

void Login::requestLogin()
{
    if (!checkInput())
        return;

    ui->pbLogin->setDisabled(true);

    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished, this, &Login::processLogin);
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    ApiManager::getInstance()->post(pNetworkManager,
                                    ApiType::LoaManager,
                                    static_cast<int>(LoamanagerApi::Signin),
                                    RequestBodyBuilder::buildLoginBody(ui->leId->text(), ui->lePassword->text()));
}

void Login::processLogin(QNetworkReply *pReply)
{
    int statusCode = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (statusCode == 201) {
        ApiManager::getInstance()->setAccessToken(pReply->readAll());

        mpLoginButton->setText("로그아웃");

        this->close();
    } else {
        const QString message = QJsonDocument::fromJson(pReply->readAll())
                                .object()
                                .find("message")->toString();

        QMessageBox msgBox;
        msgBox.setText(message);
        msgBox.exec();

        ui->pbLogin->setEnabled(true);
    }
}

bool Login::checkInput()
{
    if (ui->leId->text().isEmpty() || ui->lePassword->text().isEmpty())
        return false;
    else
        return true;
}
