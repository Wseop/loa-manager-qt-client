#include "smart_search.h"
#include "ui_smart_search.h"
#include "resource/resource_manager.h"
#include "ui/widget_manager.h"
#include "function/smart_search/smart_search_menu.h"
#include "function/smart_search/smart_search_engravebook.h"
#include "function/smart_search/smart_search_reforge.h"
#include "function/smart_search/smart_search_gem.h"
#include "function/smart_search/smart_search_accessory.h"
#include "function/smart_search/smart_search_abilitystone.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QPushButton>

SmartSearch* SmartSearch::m_pInstance = nullptr;

SmartSearch::SmartSearch() :
    ui(new Ui::SmartSearch),
    m_pSelectedMenu(nullptr)
{
    ui->setupUi(this);
    ui->hLayoutMenu->setAlignment(Qt::AlignHCenter);

    loadResource();
    initMenu();
}

SmartSearch::~SmartSearch()
{
    for (QWidget* pWidget : m_widgets)
        delete pWidget;
    for (SmartSearchMenu* pMenu : m_menuWidgets)
        delete pMenu;
    delete ui;
}

void SmartSearch::loadResource()
{
    QJsonObject json = ResourceManager::getInstance()->loadJson("smart_search");
    m_menuNames = json.find("Menu")->toVariant().toStringList();
}

void SmartSearch::initMenu()
{
    // 메뉴별 위젯 생성 (메뉴 목록 순서대로 생성)
    SmartSearchMenu* pEngraveBook = new SmartSearchEngraveBook(ui->vLayoutMain);
    m_menuWidgets.append(pEngraveBook);
    SmartSearchMenu* pReforge = new SmartSearchReforge(ui->vLayoutMain);
    m_menuWidgets.append(pReforge);
    SmartSearchMenu* pGem = new SmartSearchGem(ui->vLayoutMain);
    m_menuWidgets.append(pGem);
    SmartSearchMenu* pAccessory = new SmartSearchAccessory(ui->vLayoutMain);
    m_menuWidgets.append(pAccessory);
    SmartSearchMenu* pAbilityStone = new SmartSearchAbilityStone(ui->vLayoutMain);
    m_menuWidgets.append(pAbilityStone);

    // 메뉴 버튼 추가
    for (int i = 0; i < m_menuNames.size(); i++)
    {
        QPushButton* pMenuButton = WidgetManager::createPushButton(m_menuNames[i]);
        ui->hLayoutMenu->addWidget(pMenuButton);
        m_widgets.append(pMenuButton);

        connect(pMenuButton, &QPushButton::released, this, [&, i, pMenuButton](){
            if (m_pSelectedMenuButton != nullptr)
                m_pSelectedMenuButton->setEnabled(true);
            m_pSelectedMenuButton = pMenuButton;
            m_pSelectedMenuButton->setDisabled(true);

            // 화면에 표시할 메뉴 변경
            if (m_pSelectedMenu != nullptr)
                dynamic_cast<QWidget*>(m_pSelectedMenu)->hide();
            m_pSelectedMenu = m_menuWidgets[i];
            dynamic_cast<QWidget*>(m_pSelectedMenu)->show();

            // 변경된 메뉴 갱신
            m_pSelectedMenu->refresh();
        });
    }
}

SmartSearch* SmartSearch::getInstace()
{
    if (m_pInstance == nullptr)
        m_pInstance = new SmartSearch();
    return m_pInstance;
}

void SmartSearch::destroyInstance()
{
    if (m_pInstance == nullptr)
        return;
    delete m_pInstance;
    m_pInstance = nullptr;
}
