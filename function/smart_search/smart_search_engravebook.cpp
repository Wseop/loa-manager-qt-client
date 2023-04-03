#include "smart_search_engravebook.h"
#include "ui_smart_search_engravebook.h"
#include "ui/widget_manager.h"
#include "api/api_manager.h"
#include "api/response_parser.h"
#include "api/lostark/search_option.h"

#include <algorithm>
#include <QLabel>
#include <QGridLayout>
#include <QFrame>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>

SmartSearchEngraveBook::SmartSearchEngraveBook(QLayout *pLayout) :
    SmartSearchMenu(),
    ui(new Ui::SmartSearchEngraveBook),
    mSearchPageNo(1)
{
    ui->setupUi(this);
    ui->gridBattleEngrave->setAlignment(Qt::AlignTop);
    ui->gridClassEngrave->setAlignment(Qt::AlignTop);

    pLayout->addWidget(this);
    this->hide();

    mLayouts = {ui->gridBattleEngrave, ui->gridClassEngrave};

    initializeUI();
}

SmartSearchEngraveBook::~SmartSearchEngraveBook()
{
    for (QWidget* pWidget : mWidgets)
        delete pWidget;
    clearUI();
    delete ui;
}

void SmartSearchEngraveBook::refresh()
{
    searchEngraveBook(true);
}

void SmartSearchEngraveBook::initializeUI()
{
    const QStringList categories = {"[전투 각인]", "[직업 각인]"};
    const QStringList attributes = {"#", "각인서", "전일 평균 거래가", "최근 거래가", "최저가"};

    // 전투, 직업 각인 table 초기 세팅
    for (int i = 0; i < categories.size(); i++)
    {
        QLabel *pLabelCategory = WidgetManager::createLabel(categories[i], 16);
        mLayouts[i]->addWidget(pLabelCategory, 0, 0, 1, -1, Qt::AlignHCenter);
        mWidgets.append(pLabelCategory);

        for (int j = 0; j < attributes.size(); j++)
        {
            QLabel *pLabelAttribute = WidgetManager::createLabel(attributes[j], 12);
            mLayouts[i]->addWidget(pLabelAttribute, 1, j);
            mWidgets.append(pLabelAttribute);
        }
    }
}

void SmartSearchEngraveBook::updateUI()
{
    clearUI();

    int rows[2] = {2, 2};

    for (const MarketItem &item : mSearchResults)
    {
        int layoutIndex = item.name.contains("[") ? 1 : 0;

        QFrame *pHLine = WidgetManager::createLine(QFrame::HLine);
        mLayouts[layoutIndex]->addWidget(pHLine, rows[layoutIndex]++, 0, 1, -1);
        mPriceWidgets.append(pHLine);

        QLabel *pIcon = WidgetManager::createIcon(":/image/item/book/0.png", nullptr);
        mLayouts[layoutIndex]->addWidget(pIcon, rows[layoutIndex], 0);
        mLayouts[layoutIndex]->setAlignment(pIcon, Qt::AlignHCenter);
        mPriceWidgets.append(pIcon);

        QLabel *pLabelName = WidgetManager::createLabel(item.name, 10, itemGradeToTextColor(ItemGrade::전설));
        mLayouts[layoutIndex]->addWidget(pLabelName, rows[layoutIndex], 1);
        mPriceWidgets.append(pLabelName);

        QLabel *pLabelYDayAvgPrice = WidgetManager::createLabel(QString("%L1").arg(item.yDayAvgPrice));
        mLayouts[layoutIndex]->addWidget(pLabelYDayAvgPrice, rows[layoutIndex], 2);
        mPriceWidgets.append(pLabelYDayAvgPrice);

        QLabel *pLabelRecentPrice = WidgetManager::createLabel(QString("%L1").arg(item.recentPrice));
        mLayouts[layoutIndex]->addWidget(pLabelRecentPrice, rows[layoutIndex], 3);
        mPriceWidgets.append(pLabelRecentPrice);

        QLabel *pLabelMinPrice = WidgetManager::createLabel(QString("%L1").arg(item.currentMinPrice));
        mLayouts[layoutIndex]->addWidget(pLabelMinPrice, rows[layoutIndex]++, 4);
        mPriceWidgets.append(pLabelMinPrice);
    }
}

void SmartSearchEngraveBook::clearUI()
{
    for (QWidget* pWidget : mPriceWidgets)
        delete pWidget;
    mPriceWidgets.clear();
}

void SmartSearchEngraveBook::searchEngraveBook(bool bResetPageNo)
{
    if (bResetPageNo)
        mSearchPageNo = 1;

    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished, this, &SmartSearchEngraveBook::parseSearchResult);
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    SearchOption searchOption(SearchType::Market);
    searchOption.setCategoryCode(CategoryCode::EngraveBook);
    searchOption.setItemGrade(ItemGrade::전설);
    searchOption.setPageNo(mSearchPageNo);
    searchOption.setSortCondition("DESC");

    ApiManager::getInstance()->post(pNetworkManager, ApiType::Lostark, static_cast<int>(LostarkApi::Market), QJsonDocument(searchOption.toJsonObject()).toJson());
}

void SmartSearchEngraveBook::parseSearchResult(QNetworkReply *pReply)
{
    QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
    if (response.isNull())
        return;

    ResponseMarket responseMarket = ResponseParser::parseMarketItem(response);
    QList<MarketItem> &items = responseMarket.items;

    for (const MarketItem &item : items)
        mSearchResults << item;

    // 다음 페이지가 있다면 추가 검색
    if (responseMarket.pageSize > mSearchPageNo)
    {
        mSearchPageNo++;
        searchEngraveBook(false);
    }
    // 마지막 페이지이면 검색 결과 ui에 반영
    else
    {
        updateUI();
    }
}
