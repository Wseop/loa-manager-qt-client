#include "loamanager.h"
#include "ui_loamanager.h"
#include "ui/widget_manager.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QPushButton>

LoaManager::LoaManager() :
    ui(new Ui::LoaManager)
{
    ui->setupUi(this);

    QFile file(":/json/setting/main.json");
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << Q_FUNC_INFO << ": mainSetting read fail";
        return;
    }
    m_mainSetting = QJsonDocument::fromJson(file.readAll()).object();
    file.close();

    this->setWindowIcon(QIcon(":/icon/Home.ico"));
    this->setWindowTitle(m_mainSetting.find("Version")->toString());
    this->showMaximized();

    setLayoutAlignment();
    createMenuButtons();
}

LoaManager::~LoaManager()
{
    delete ui;
}

void LoaManager::setLayoutAlignment()
{
    ui->hLayoutMenu->setAlignment(Qt::AlignLeft);
}

void LoaManager::createMenuButtons()
{
    const QJsonArray& menus = m_mainSetting.find("Menu")->toArray();

    for (const QJsonValue& menu : menus)
    {
        const QJsonObject& menuObj = menu.toObject();
        QPushButton* pButton = WidgetManager::createPushButton(menuObj.find("Name")->toString(), MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, 10, this);
        ui->hLayoutMenu->addWidget(pButton);
    }
}

