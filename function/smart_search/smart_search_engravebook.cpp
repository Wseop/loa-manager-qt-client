#include "smart_search_engravebook.h"
#include "ui_smart_search_engravebook.h"
#include "ui/widget_manager.h"
#include "api/response_parser.h"
#include "api/api_manager.h"
#include "api/search_option.h"

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
    const QStringList attributes = {"#", "각인서", "최근 거래가", "최저가"};

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

    const QList<QStringList*> engraveKeys = {&mBattleEngraveKeys, &mClassEngraveKeys};

    for (int i = 0; i < engraveKeys.size(); i++)
    {
        int row = 2;
        const QStringList *keys = engraveKeys[i];

        for (const QString& key : *keys)
        {
            const int &recentPrice = mEngravePrices[key].first;
            const int &minPrice = mEngravePrices[key].second;

            QFrame *pHLine = WidgetManager::createLine(QFrame::HLine);
            mLayouts[i]->addWidget(pHLine, row++, 0, 1, -1);
            mPriceWidgets.append(pHLine);

            QLabel *pIcon = WidgetManager::createIcon(":/image/item/book/0.png", nullptr);
            mLayouts[i]->addWidget(pIcon, row, 0);
            mLayouts[i]->setAlignment(pIcon, Qt::AlignHCenter);
            mPriceWidgets.append(pIcon);

            QLabel *pLabelName = WidgetManager::createLabel(key, 10, itemGradeToTextColor(ItemGrade::전설));
            mLayouts[i]->addWidget(pLabelName, row, 1);
            mPriceWidgets.append(pLabelName);

            QLabel *pLabelRecentPrice = WidgetManager::createLabel(QString("%L1").arg(recentPrice), 10);
            mLayouts[i]->addWidget(pLabelRecentPrice, row, 2);
            mPriceWidgets.append(pLabelRecentPrice);

            QLabel *pLabelMinPrice = WidgetManager::createLabel(QString("%L1").arg(minPrice), 10);
            mLayouts[i]->addWidget(pLabelMinPrice, row++, 3);
            mPriceWidgets.append(pLabelMinPrice);
        }
    }

    mBattleEngraveKeys.clear();
    mClassEngraveKeys.clear();
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

    ApiManager::getInstance()->post(pNetworkManager, LostarkApi::Market, QJsonDocument(searchOption.toJsonObject()).toJson());
}

void SmartSearchEngraveBook::parseSearchResult(QNetworkReply *pReply)
{
    QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
    if (response.isNull())
        return;

    // 검색 결과 parsing
    ResponseMarket responseMarket = ResponseParser::parseMarketItem(response);

    QList<MarketItem> &items = responseMarket.items;

    for (const MarketItem &item : items)
    {
        if (item.name[0] == '[')
            mClassEngraveKeys << item.name;
        else
            mBattleEngraveKeys << item.name;

        mEngravePrices[item.name] = {item.recentPrice, item.currentMinPrice};
    }

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
