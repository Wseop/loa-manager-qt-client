#include "loamanager.h"
#include "ui_loamanager.h"
#include "ui/widget_manager.h"
#include "ui/admin_login.h"
#include "function/function_widget.h"
#include "function/auction_calculator/auction_calculator.h"
#include "function/smart_search/smart_search.h"
#include "function/content_reward/content_reward.h"
#include "function/raid_profit/raid_profit.h"
#include "resource/resource_manager.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QPushButton>
#include <QGroupBox>
#include <QHBoxLayout>

bool gbAdmin = false;

LoaManager::LoaManager() :
    ui(new Ui::LoaManager),
    mMainSetting(ResourceManager::getInstance()->loadJson("main")),
    mpAdminLogin(new AdminLogin()),
    mpAdminButton(nullptr)
{
    ui->setupUi(this);
    ui->hLayoutMenu->setAlignment(Qt::AlignLeft);
    ui->vLayoutContents->setAlignment(Qt::AlignTop);

    initFunction();
    initMenuButton();
    initAdminButton();

    this->setWindowIcon(QIcon(":/Home.ico"));
    this->setWindowTitle(mMainSetting.find("Version")->toString());
    this->showMaximized();
}

LoaManager::~LoaManager()
{
    delete mpAdminLogin;
    delete mpAdminButton;

    for (QPushButton* pButton : mMenuButtons)
        delete pButton;
    mMenuButtons.clear();

    for (QWidget* pWidget : mWidgets)
        delete pWidget;
    mWidgets.clear();

    delete ui;
}

void LoaManager::initFunction()
{
    // main.json의 메뉴 list 순서에 맞게 등록
    mFunctions.append(SmartSearch::getInstance());
    mFunctions.append(AuctionCalculator::getInstance());
    mFunctions.append(ContentReward::getInstance());
    mFunctions.append(RaidProfit::getInstance());

    for (QWidget* pWidget : mFunctions)
    {
        pWidget->hide();
        ui->vLayoutContents->addWidget(pWidget);
    }
}

void LoaManager::initMenuButton()
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

void LoaManager::initAdminButton()
{
    mpAdminButton = WidgetManager::createPushButton("관리자 로그인");
    ui->hLayoutAdmin->addWidget(mpAdminButton);
    ui->hLayoutAdmin->setAlignment(Qt::AlignRight);
    connect(mpAdminButton, &QPushButton::released, this, [&](){
        mpAdminLogin->show();
    });
}

