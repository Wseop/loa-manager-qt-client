#include "smart_search_gem.h"
#include "ui_smart_search_gem.h"
#include "ui/widget_manager.h"
#include "api/api_manager.h"
#include "api/search_option.h"
#include "game/item/gem.h"

#include <QLabel>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>

SmartSearchGem::SmartSearchGem(QLayout* pLayout) :
    ui(new Ui::SmartSearchGem),
    m_searchList({
        {"5레벨 멸화", "6레벨 멸화", "7레벨 멸화", "8레벨 멸화", "9레벨 멸화", "10레벨 멸화"},
        {"5레벨 홍염", "6레벨 홍염", "7레벨 홍염", "8레벨 홍염", "9레벨 홍염", "10레벨 홍염"}
    })
{
    ui->setupUi(this);

    pLayout->addWidget(this);
    this->hide();

    initializeUI();
}

SmartSearchGem::~SmartSearchGem()
{
    for (QWidget* pWidget : m_widgets)
        delete pWidget;
    clearUI();
    delete ui;
}

void SmartSearchGem::refresh()
{
    clearUI();

    for (int i = 0; i < m_searchList.size(); i++)
    {
        for (int j = 0; j < m_searchList[i].size(); j++)
        {
            QNetworkAccessManager* pNetworkManager = new QNetworkAccessManager();
            connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&](QNetworkReply* pReply){
                // 검색 결과 parsing
                QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
                if (response.isNull())
                    return;

                const QJsonObject& item = response.object().find("Items")->toArray()[0].toObject();
                const QString& itemName = item.find("Name")->toString();

                // 보석 타입 설정
                GemType gemType;
                if (itemName.contains("멸화"))
                    gemType = GemType::멸화;
                else
                    gemType = GemType::홍염;

                Gem gem(gemType);
                gem.setItemName(itemName);
                gem.setItemGrade(qStringToItemGrade(item.find("Grade")->toString()));
                gem.setIconPath(item.find("Icon")->toString());

                // 보석 레벨 설정
                if (itemName.startsWith("10"))
                    gem.setGemLevel(10);
                else
                    gem.setGemLevel(itemName[0].digitValue());

                // 가격 parsing
                const int& price = item.find("AuctionInfo")->toObject().find("BuyPrice")->toInt();

                updateUI(gem, price);
            });
            connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

            // 보석 가격 검색
            SearchOption searchOption(SearchType::Auction);
            searchOption.setCategoryCode(CategoryCode::Gem);
            searchOption.setItemName(m_searchList[i][j]);
            searchOption.setItemTier(3);
            searchOption.setPageNo(1);
            searchOption.setSortCondition("ASC");

            ApiManager::getInstance()->post(pNetworkManager, LostarkApi::Auction, QJsonDocument(searchOption.toJsonObject()).toJson());
        }
    }
}

void SmartSearchGem::initializeUI()
{
    const QStringList attributes = {"#", "보석", "최저가"};
    const QList<QGridLayout*> layouts = {ui->gridLeft, ui->gridRight};

    for (int i = 0; i < layouts.size(); i++)
    {
        // attribute label 추가
        for (int j = 0; j < attributes.size(); j++)
        {
            QLabel* pLabel = WidgetManager::createLabel(attributes[j], 14);
            layouts[i]->addWidget(pLabel, 0, j);
            m_widgets.append(pLabel);
        }
    }
}

void SmartSearchGem::updateUI(const Gem gem, const int price)
{
    QGridLayout* pLayout = gem.gemType() == GemType::멸화 ? ui->gridLeft : ui->gridRight;
    int row = (2 * gem.gemLevel()) - 9;

    QNetworkAccessManager* pIconLoader = new QNetworkAccessManager();
    m_iconLoaders.append(pIconLoader);

    QFrame* pHLine = WidgetManager::createLine(QFrame::HLine);
    pLayout->addWidget(pHLine, row++, 0, 1, -1);
    m_gemWidgets.append(pHLine);

    QLabel* pIcon = WidgetManager::createIcon(gem.iconPath(), pIconLoader, itemGradeToBGColor(gem.itemGrade()));
    pLayout->addWidget(pIcon, row, 0);
    m_gemWidgets.append(pIcon);

    QLabel* pLabelName = WidgetManager::createLabel(gem.itemName(), 10, itemGradeToTextColor(gem.itemGrade()));
    pLayout->addWidget(pLabelName, row, 1);
    m_gemWidgets.append(pLabelName);

    QLabel* pLabelPrice = WidgetManager::createLabel(QString("%L1").arg(price), 10);
    pLayout->addWidget(pLabelPrice, row, 2);
    m_gemWidgets.append(pLabelPrice);
}

void SmartSearchGem::clearUI()
{
    for (QWidget* pWidget : m_gemWidgets)
        delete pWidget;
    for (QNetworkAccessManager* pNetworkManager : m_iconLoaders)
        delete pNetworkManager;

    m_gemWidgets.clear();
    m_iconLoaders.clear();
}
