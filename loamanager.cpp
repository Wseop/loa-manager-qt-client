#include "loamanager.h"
#include "ui_loamanager.h"
#include "ui/widget_manager.h"
#include "ui/admin_login.h"
#include "function/character_search/character_search.h"
#include "function/character_ranking/character_ranking.h"
#include "function/setting_ranking/setting_ranking.h"
#include "function/auction_calculator/auction_calculator.h"
#include "function/quotation/tripod/tripod_quotation.h"
#include "function/raid/raidreward_profit.h"
#include "function/contents_reward/contents_reward.h"
#include "function/smart_search/smart_search.h"
#include "resource/resource_manager.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QPushButton>
#include <QGroupBox>
#include <QHBoxLayout>

bool g_bAdmin = false;

LoaManager::LoaManager() :
    ui(new Ui::LoaManager),
    m_mainSetting(ResourceManager::getInstance()->loadJson("main")),
    m_pAdminLogin(new AdminLogin()),
    m_pAdminButton(nullptr)
{
    ui->setupUi(this);
    ui->hLayoutMenu->setAlignment(Qt::AlignLeft);
    ui->vLayoutContents->setAlignment(Qt::AlignTop);

    initFunction();
    initMenuButton();
    initAdminButton();

    this->setWindowIcon(QIcon(":/icon/Home.ico"));
    this->setWindowTitle(m_mainSetting.find("Version")->toString());
    this->showMaximized();
}

LoaManager::~LoaManager()
{
    delete m_pAdminLogin;
    delete m_pAdminButton;
    for (QWidget* pWidget : m_widgets)
        delete pWidget;
    delete ui;
}

void LoaManager::initFunction()
{
    // main.json의 메뉴 list 순서에 맞게 등록
    m_functions.append(CharacterSearch::getInstance());
    m_functions.append(CharacterRanking::getInstance());
    m_functions.append(SettingRanking::getInstance());
    m_functions.append(AuctionCalculator::getInstance());
    m_functions.append(TripodQuotation::getInstance());
    m_functions.append(RaidRewardProfit::getInstance());
    m_functions.append(ContentsReward::getInstance());
    m_functions.append(SmartSearch::getInstace());

    for (QWidget* pWidget : m_functions)
    {
        pWidget->hide();
        ui->vLayoutContents->addWidget(pWidget);
    }
}

void LoaManager::initMenuButton()
{
    int menuIndex = 0;
    const QJsonArray& menus = m_mainSetting.find("Menu")->toArray();

    for (int i = 0; i < menus.size(); i++)
    {
        const QJsonObject& menu = menus[i].toObject();
        const QString& title = menu.find("Title")->toString();
        const QStringList& menuList = menu.find("List")->toVariant().toStringList();

        QGroupBox* pMenuGroup = WidgetManager::createGroupBox(title);
        ui->hLayoutMenu->addWidget(pMenuGroup);
        m_widgets.append(pMenuGroup);
        QHBoxLayout* pLayout = new QHBoxLayout();
        pMenuGroup->setLayout(pLayout);

        for (const QString& menuName : menuList)
        {
            QPushButton* pMenuButton = WidgetManager::createPushButton(menuName);
            pLayout->addWidget(pMenuButton);
            m_widgets.append(pMenuButton);

            connect(pMenuButton, &QPushButton::released, this, [&, menuIndex](){
                for (int i = 0; i < m_functions.size(); i++)
                {
                    if (i == menuIndex)
                        m_functions[i]->show();
                    else
                        m_functions[i]->hide();
                }
            });

            menuIndex++;
        }
    }
}

void LoaManager::initAdminButton()
{
    m_pAdminButton = WidgetManager::createPushButton("관리자 로그인");
    ui->hLayoutAdmin->addWidget(m_pAdminButton);
    ui->hLayoutAdmin->setAlignment(Qt::AlignRight);
    connect(m_pAdminButton, &QPushButton::released, this, [&](){
        m_pAdminLogin->show();
    });
}

