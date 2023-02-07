#include "loamanager.h"
#include "ui_loamanager.h"
#include "ui/widget_manager.h"
#include "function/character_search/character_search.h"
#include "function/character_ranking/character_ranking.h"
#include "function/setting_ranking/setting_ranking.h"
#include "function/auction_calculator/auction_calculator.h"
#include "function/quotation/tripod/tripod_quotation.h"
#include "function/quotation/abilitystone/abilitystone_quotation.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QPushButton>

LoaManager::LoaManager() :
    ui(new Ui::LoaManager),
    m_pBackButton(nullptr)
{
    ui->setupUi(this);

    QFile file(":/json/json/main.json");
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << Q_FUNC_INFO << ": mainSetting read fail";
        return;
    }
    m_mainSetting = QJsonDocument::fromJson(file.readAll()).object();
    file.close();

    setLayoutAlignment();
    createMenuButtons();
    addFunctions();
    initConnects();

    this->setWindowIcon(QIcon(":/icon/Home.ico"));
    this->setWindowTitle(m_mainSetting.find("Version")->toString());
    this->showMaximized();
}

LoaManager::~LoaManager()
{
    delete ui;
}

void LoaManager::setLayoutAlignment()
{
    ui->hLayoutMenu->setAlignment(Qt::AlignLeft);
    ui->vLayoutContents->setAlignment(Qt::AlignTop);
}

void LoaManager::createMenuButtons()
{
    // create back button
    m_pBackButton = WidgetManager::createPushButton("");
    m_pBackButton->setIcon(QPixmap(":/icon/image/back.png"));
    connect(m_pBackButton, &QPushButton::released, this, [&](){
        // hide child menu & show parent menu
        for (QPushButton* pButton : m_childMenuButtons)
            pButton->hide();
        for (QPushButton* pButton : m_parentMenuButtons)
            pButton->show();
        m_pBackButton->hide();
    });
    ui->hLayoutMenu->addWidget(m_pBackButton);
    m_pBackButton->hide();

    // create menu buttons
    const QJsonArray& menus = m_mainSetting.find("Menu")->toArray();
    for (const QJsonValue& menu : menus)
    {
        const QJsonObject& menuObj = menu.toObject();
        // create parent_menu button
        QPushButton* pParentButton = WidgetManager::createPushButton(menuObj.find("Title")->toString());
        connect(pParentButton, &QPushButton::released, this, [&, pParentButton](){
            // show child buttons & hide parent buttons
            const auto childButtons = m_parentToChildButtons[pParentButton];
            for (QPushButton* pButton : childButtons)
                pButton->show();
            for (QPushButton* pButton : m_parentMenuButtons)
                pButton->hide();
            m_pBackButton->show();
        });
        ui->hLayoutMenu->addWidget(pParentButton);
        m_parentMenuButtons.append(pParentButton);

        // create child_menu button
        const QJsonArray& childMenus = menuObj.find("List")->toArray();
        for (const QJsonValue& childMenu : childMenus)
        {
            const QString& menuName = childMenu.toString();
            QPushButton* pChildButton = WidgetManager::createPushButton(menuName);
            ui->hLayoutMenu->addWidget(pChildButton);
            pChildButton->hide();
            m_childMenuButtons[menuName] = pChildButton;
            m_parentToChildButtons[pParentButton].append(pChildButton);
        }
    }
}

void LoaManager::initConnects()
{
    connect(m_childMenuButtons["캐릭터 조회"], &QPushButton::released, this, [&](){
        for (QWidget* pWidget : m_functions)
            pWidget->hide();
        CharacterSearch::getInstance()->show();
    });
    connect(m_childMenuButtons["캐릭터 순위"], &QPushButton::released, this, [&](){
        for (QWidget* pWidget : m_functions)
            pWidget->hide();
        CharacterRanking::getInstance()->show();
        if (!CharacterRanking::getInstance()->loaded())
            emit CharacterRanking::getInstance()->refresh();
    });
    connect(m_childMenuButtons["세팅 순위"], &QPushButton::released, this, [&](){
        for (QWidget* pWidget : m_functions)
            pWidget->hide();
        SettingRanking::getInstance()->show();
    });
    connect(m_childMenuButtons["경매 손익\n계산기"], &QPushButton::released, this, [&](){
        for (QWidget* pWidget : m_functions)
            pWidget->hide();
        AuctionCalculator::getInstance()->show();
    });
    connect(m_childMenuButtons["트라이포드\n조회 (5레벨)"], &QPushButton::released, this, [&](){
        for (QWidget* pWidget : m_functions)
            pWidget->hide();
        TripodQuotation::getInstance()->show();
    });
    connect(m_childMenuButtons["어빌리티 스톤\n조회"], &QPushButton::released, this, [&](){
        for (QWidget* pWidget : m_functions)
            pWidget->hide();
        AbilityStoneQuotation::getInstance()->show();
    });
}

void LoaManager::addFunctions()
{
    m_functions.append(CharacterSearch::getInstance());
    m_functions.append(CharacterRanking::getInstance());
    m_functions.append(SettingRanking::getInstance());
    m_functions.append(AuctionCalculator::getInstance());
    m_functions.append(TripodQuotation::getInstance());
    m_functions.append(AbilityStoneQuotation::getInstance());

    for (QWidget* pWidget : m_functions)
    {
        pWidget->hide();
        ui->vLayoutContents->addWidget(pWidget);
    }
}

