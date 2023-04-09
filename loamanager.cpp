#include "loamanager.h"
#include "ui_loamanager.h"
#include "ui/widget_manager.h"
#include "ui/admin_login.h"
#include "function/function_widget.h"
#include "function/character_search/character_search.h"
#include "function/setting_ranking/setting_ranking.h"
#include "function/auction_calculator/auction_calculator.h"
#include "function/smart_search/smart_search.h"
#include "function/content_reward/content_reward.h"
#include "function/raid_profit/raid_profit.h"
#include "resource/resource_manager.h"
#include "api/api_manager.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QNetworkAccessManager>
#include <QNetworkReply>

bool gbAdmin = false;

LoaManager::LoaManager() :
    ui(new Ui::LoaManager),
    mMainSetting(ResourceManager::getInstance()->loadJson("main")),
    mpAdminLogin(new AdminLogin()),
    mpAdminButton(nullptr),
    mpLabelVersionInfo(nullptr)
{
    ui->setupUi(this);
    ui->hLayoutMenu->setAlignment(Qt::AlignLeft);
    ui->vLayoutContents->setAlignment(Qt::AlignTop);

    initializeFunction();
    initializeMenuButton();
    initializeVersionInfo();
    initializeAdminButton();

    this->setWindowIcon(QIcon(":/Home.ico"));
    this->setWindowTitle(mMainSetting.find("Version")->toString());
    this->showMaximized();
}

LoaManager::~LoaManager()
{
    delete mpAdminLogin;
    delete mpAdminButton;

    delete mpLabelVersionInfo;

    for (QPushButton* pButton : mMenuButtons)
        delete pButton;
    mMenuButtons.clear();

    for (QWidget* pWidget : mWidgets)
        delete pWidget;
    mWidgets.clear();

    delete ui;
}

void LoaManager::initializeFunction()
{
    // main.json의 메뉴 list 순서에 맞게 등록
    mFunctions << CharacterSearch::getInstance();
    mFunctions << SettingRanking::getInstance();
    mFunctions << SmartSearch::getInstance();
    mFunctions << AuctionCalculator::getInstance();
    mFunctions << ContentReward::getInstance();
    mFunctions << RaidProfit::getInstance();

    for (QWidget* pWidget : mFunctions)
    {
        pWidget->hide();
        ui->vLayoutContents->addWidget(pWidget);
    }
}

void LoaManager::initializeMenuButton()
{
    int menuIndex = 0;
    const QJsonArray &menus = mMainSetting.find("Menu")->toArray();

    for (int i = 0; i < menus.size(); i++)
    {
        const QJsonObject &menu = menus[i].toObject();
        const QString &title = menu.find("Title")->toString();
        const QStringList &menuList = menu.find("List")->toVariant().toStringList();

        QGroupBox *pMenuGroup = WidgetManager::createGroupBox(title);
        ui->hLayoutMenu->addWidget(pMenuGroup);
        mWidgets.append(pMenuGroup);
        QHBoxLayout *pLayout = new QHBoxLayout();
        pMenuGroup->setLayout(pLayout);

        for (const QString& menuName : menuList)
        {
            QPushButton *pMenuButton = WidgetManager::createPushButton(menuName);
            pLayout->addWidget(pMenuButton);
            mMenuButtons.append(pMenuButton);

            connect(pMenuButton, &QPushButton::released, this, [&, menuIndex](){
                for (int i = 0; i < mFunctions.size(); i++)
                {
                    if (i == menuIndex)
                    {
                        mMenuButtons[i]->setDisabled(true);
                        dynamic_cast<FunctionWidget*>(mFunctions[i])->start();
                        mFunctions[i]->show();
                    }
                    else
                    {
                        mMenuButtons[i]->setEnabled(true);
                        mFunctions[i]->hide();
                    }
                }
            });

            menuIndex++;
        }
    }
}

void LoaManager::initializeAdminButton()
{
    mpAdminButton = WidgetManager::createPushButton("관리자 로그인");
    ui->hLayoutAdmin->addWidget(mpAdminButton);
    ui->hLayoutAdmin->setAlignment(Qt::AlignRight);
    connect(mpAdminButton, &QPushButton::released, this, [&](){
        mpAdminLogin->show();
    });
}

void LoaManager::initializeVersionInfo()
{
    const QString &versionInfo = mMainSetting.find("Version")->toString();

    mpLabelVersionInfo = WidgetManager::createLabel("", 10, "", 500, 50);
    ui->hLayoutAdmin->addWidget(mpLabelVersionInfo);

    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();

    connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&, versionInfo](QNetworkReply *pReply)
    {
        QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());

        if (response.isNull())
        {
            mpLabelVersionInfo->setText("Failed to load version info.");
            return;
        }

        if (versionInfo == response.object().find("key")->toString())
        {
            mpLabelVersionInfo->setText("최신 버전입니다.");
            mpLabelVersionInfo->setStyleSheet("QLabel { color: blue }");
        }
        else
        {
            mpLabelVersionInfo->setText("업데이트가 필요합니다.\n(https://github.com/Wseop/QT_LoaManager/releases)");
            mpLabelVersionInfo->setStyleSheet("QLabel { color: red }");
        }
    });
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    ApiManager::getInstance()->get(pNetworkManager, ApiType::LoaManager, static_cast<int>(LoamanagerApi::Admin), "1");
}

