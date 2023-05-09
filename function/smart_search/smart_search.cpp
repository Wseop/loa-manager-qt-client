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
#include "function/smart_search/smart_search_tripod.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QPushButton>

SmartSearch *SmartSearch::mpInstance = nullptr;

SmartSearch::SmartSearch() :
    ui(new Ui::SmartSearch),
    mpSelectedMenu(nullptr),
    mpSelectedMenuButton(nullptr)
{
    ui->setupUi(this);
    ui->hLayoutMenu->setAlignment(Qt::AlignHCenter);

    loadResource();
    initializeMenu();
}

SmartSearch::~SmartSearch()
{
    delete ui;
}

void SmartSearch::loadResource()
{
    QJsonObject json = ResourceManager::getInstance()->loadJson("smart_search");
    mMenuNames = json.find("Menu")->toVariant().toStringList();
}

void SmartSearch::initializeMenu()
{
    // 메뉴별 위젯 생성 (메뉴 목록 순서대로 생성)
    SmartSearchMenu *pEngraveBook = new SmartSearchEngraveBook(ui->vLayoutMain);
    mMenuWidgets.append(pEngraveBook);
    SmartSearchMenu *pReforge = new SmartSearchReforge(ui->vLayoutMain);
    mMenuWidgets.append(pReforge);
    SmartSearchMenu *pGem = new SmartSearchGem(ui->vLayoutMain);
    mMenuWidgets.append(pGem);
    SmartSearchMenu *pAccessory = new SmartSearchAccessory(ui->vLayoutMain);
    mMenuWidgets.append(pAccessory);
    SmartSearchMenu *pAbilityStone = new SmartSearchAbilityStone(ui->vLayoutMain);
    mMenuWidgets.append(pAbilityStone);
    SmartSearchMenu *pTripod = new SmartSearchTripod(ui->vLayoutMain);
    mMenuWidgets.append(pTripod);

    // 메뉴 버튼 추가
    for (int i = 0; i < mMenuNames.size(); i++)
    {
        QPushButton *pMenuButton = WidgetManager::createPushButton(mMenuNames[i]);
        ui->hLayoutMenu->addWidget(pMenuButton);

        connect(pMenuButton, &QPushButton::released, this, [&, i, pMenuButton](){
            if (mpSelectedMenuButton != nullptr)
                mpSelectedMenuButton->setEnabled(true);
            mpSelectedMenuButton = pMenuButton;
            mpSelectedMenuButton->setDisabled(true);

            // 화면에 표시할 메뉴 변경
            if (mpSelectedMenu != nullptr)
                dynamic_cast<QWidget*>(mpSelectedMenu)->hide();
            mpSelectedMenu = mMenuWidgets[i];
            dynamic_cast<QWidget*>(mpSelectedMenu)->show();

            // 변경된 메뉴 갱신
            mpSelectedMenu->refresh();
        });
    }
}

SmartSearch *SmartSearch::getInstance()
{
    if (mpInstance == nullptr)
        mpInstance = new SmartSearch();
    return mpInstance;
}

void SmartSearch::destroyInstance()
{
    if (mpInstance == nullptr)
        return;
    delete mpInstance;
    mpInstance = nullptr;
}

void SmartSearch::start()
{

}
