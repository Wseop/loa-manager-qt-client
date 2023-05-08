#include "smart_search_gem.h"
#include "ui_smart_search_gem.h"
#include "ui/widget_manager.h"
#include "api/api_manager.h"
#include "api/response_parser.h"
#include "api/lostark/search_option.h"
#include "game/item/gem.h"

#include <QLabel>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>

SmartSearchGem::SmartSearchGem(QLayout *pLayout) :
    ui(new Ui::SmartSearchGem),
    mSearchList({
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
    delete ui;
}

void SmartSearchGem::refresh()
{
    clearUI();

    for (int i = 0; i < mSearchList.size(); i++)
    {
        for (int j = 0; j < mSearchList[i].size(); j++)
        {
            QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
            connect(pNetworkManager, &QNetworkAccessManager::finished, this, &SmartSearchGem::parseSearchResult);
            connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

            // 보석 가격 검색
            SearchOption searchOption(SearchType::Auction);
            searchOption.setCategoryCode(CategoryCode::Gem);
            searchOption.setItemName(mSearchList[i][j]);
            searchOption.setItemTier(3);
            searchOption.setPageNo(1);
            searchOption.setSortCondition("ASC");

            ApiManager::getInstance()->post(pNetworkManager,
                                            ApiType::Lostark,
                                            static_cast<int>(LostarkApi::Auction),
                                            {},
                                            QJsonDocument(searchOption.toJsonObject()).toJson());
        }
    }
}

void SmartSearchGem::initializeUI()
{
    const QStringList attributes[2] = {{"#", "멸화", "최소 입찰가", "즉시 구매가"}, {"#", "홍염", "최소 입찰가", "즉시 구매가"}};
    const QList<QGridLayout*> layouts = {ui->gridLeft, ui->gridRight};

    for (int i = 0; i < layouts.size(); i++)
    {
        // attribute label 추가
        for (int j = 0; j < attributes[i].size(); j++)
        {
            QLabel *pLabel = WidgetManager::createLabel(attributes[i][j], 14, 200, 50);
            layouts[i]->addWidget(pLabel, 0, j, Qt::AlignHCenter);
        }
    }
}

void SmartSearchGem::updateUI(const Gem gem, AuctionInfo auctionInfo)
{
    QGridLayout *pLayout = gem.gemType() == GemType::멸화 ? ui->gridLeft : ui->gridRight;
    int row = (2 * gem.gemLevel()) - 9;

    QNetworkAccessManager *pIconLoader = new QNetworkAccessManager();
    connect(pIconLoader, &QNetworkAccessManager::finished, pIconLoader, &QNetworkAccessManager::deleteLater);

    QFrame *pHLine = WidgetManager::createLine(QFrame::HLine);
    pLayout->addWidget(pHLine, row++, 0, 1, -1);
    mGemWidgets.append(pHLine);

    int col = 0;

    QLabel *pIcon = WidgetManager::createIcon(gem.iconPath(), pIconLoader, itemGradeToBGColor(gem.itemGrade()));
    pLayout->addWidget(pIcon, row, col++, Qt::AlignHCenter);
    mGemWidgets.append(pIcon);

    QLabel *pLabelName = WidgetManager::createLabel(gem.itemName(), 10, 300);
    pLabelName->setStyleSheet(QString("QLabel { color: %1 }").arg(itemGradeToTextColor(gem.itemGrade())));
    pLayout->addWidget(pLabelName, row, col++, Qt::AlignHCenter);
    mGemWidgets.append(pLabelName);

    QLabel *pLabelBidStart = WidgetManager::createLabel(QString("%L1").arg(auctionInfo.bidStartPrice), 10, 300, 50);
    pLayout->addWidget(pLabelBidStart, row, col++, Qt::AlignHCenter);
    mGemWidgets.append(pLabelBidStart);

    QLabel *pLabelBuyPrice = WidgetManager::createLabel(QString("%L1").arg(auctionInfo.buyPrice), 10, 300, 50);
    pLayout->addWidget(pLabelBuyPrice, row, col, Qt::AlignHCenter);
    mGemWidgets.append(pLabelBuyPrice);
}

void SmartSearchGem::clearUI()
{
    for (QWidget* pWidget : mGemWidgets)
        delete pWidget;
    mGemWidgets.clear();
}

void SmartSearchGem::parseSearchResult(QNetworkReply *pReply)
{
    // 검색 결과 parsing
    QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
    if (response.isNull())
        return;

    ResponseAuction responseAuction = ResponseParser::parseAuctionItem(response);
    const AuctionItem &item = responseAuction.items.front();

    // parsing 결과를 기반으로 보석 정보 생성
    GemType gemType;
    if (item.name.contains("멸화"))
        gemType = GemType::멸화;
    else
        gemType = GemType::홍염;

    Gem gem(gemType);
    gem.setItemName(item.name);
    gem.setItemGrade(qStringToItemGrade(item.grade));
    gem.setIconPath(item.icon);

    if (item.name.startsWith("10"))
        gem.setGemLevel(10);
    else
        gem.setGemLevel(item.name[0].digitValue());

    updateUI(gem, item.auctionInfo);
}
