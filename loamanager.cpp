#include "loamanager.h"
#include "ui_loamanager.h"
#include "ui/widget_manager.h"
#include "function/function_widget.h"
#include "function/character_search/character_search.h"
#include "function/setting_ranking/setting_ranking.h"
#include "function/skill_stats/skill_stats.h"
#include "function/auction_calculator/auction_calculator.h"
#include "resource/resource_manager.h"
#include "api/api_manager.h"
#include "user/login.h"
#include "game/engrave/engrave_manager.h"
#include "game/skill/skill_manager.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QNetworkAccessManager>
#include <QNetworkReply>

LoaManager::LoaManager() :
    ui(new Ui::LoaManager),
    mMainSetting(ResourceManager::getInstance()->loadJson("main")),
    mpLoginButton(nullptr),
    mpLabelVersionInfo(nullptr)
{
    EngraveManager::getInstance();
    SkillManager::getInstance();

    ui->setupUi(this);
    ui->hLayoutMenu->setAlignment(Qt::AlignLeft);
    ui->vLayoutContents->setAlignment(Qt::AlignTop);

    initializeFunction();
    initializeMenuButton();
    initializeVersionInfo();
    initializeLoginButton();

    this->setWindowIcon(QIcon(":/Home.ico"));
    this->setWindowTitle(mMainSetting.find("Version")->toString());
}

LoaManager::~LoaManager()
{
    delete ui;
}

void LoaManager::initializeFunction()
{
    // main.json의 메뉴 list 순서에 맞게 등록
    mFunctions << CharacterSearch::getInstance();
    mFunctions << SettingRanking::getInstance();
    mFunctions << SkillStats::getInstance();
    mFunctions << AuctionCalculator::getInstance();

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

void LoaManager::initializeLoginButton()
{
    mpLoginButton = WidgetManager::createPushButton("로그인");

    ui->hLayoutAdmin->addWidget(mpLoginButton);
    ui->hLayoutAdmin->setAlignment(Qt::AlignRight);

    connect(mpLoginButton, &QPushButton::released, this, [&]()
    {
        if (mpLoginButton->text() == "로그인") {
            Login *login = new Login(mpLoginButton);
            login->show();
        } else {
            ApiManager::getInstance()->setAccessToken(QString());
            mpLoginButton->setText("로그인");
        }
    });
}

void LoaManager::initializeVersionInfo()
{
    const QString &currentVersion = mMainSetting.find("Version")->toString();

    mpLabelVersionInfo = WidgetManager::createLabel("", 10, 500, 50);
    ui->hLayoutAdmin->addWidget(mpLabelVersionInfo);

    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();

    connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&, currentVersion](QNetworkReply *pReply)
    {
        QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());

        if (response.isNull())
        {
            mpLabelVersionInfo->setText("Failed to load version info.");
            return;
        }

        const QJsonArray &datas = response.array();
        QString latestVersion;
        QString releaseUrl;

        for (const QJsonValue &data : datas) {
            const QString &key = data.toObject().find("key")->toString();
            const QString &value = data.toObject().find("value")->toString();

            if (key == "version") {
                latestVersion = value;
            } else if (key == "release") {
                releaseUrl = value;
            }
        }

        if (currentVersion == latestVersion)
        {
            mpLabelVersionInfo->setText(latestVersion);
            mpLabelVersionInfo->setStyleSheet("QLabel { color: blue }");
        }
        else
        {
            mpLabelVersionInfo->setText(QString("<a href=\"%1\">%2 released\n</a>").arg(releaseUrl, latestVersion));
            mpLabelVersionInfo->setOpenExternalLinks(true);
        }
    });
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    ApiManager::getInstance()->getInfos(pNetworkManager);
}

