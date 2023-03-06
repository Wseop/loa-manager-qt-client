#include "smart_search_engravebook.h"
#include "ui_smart_search_engravebook.h"
#include "ui/widget_manager.h"
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
    m_searchPageNo(1)
{
    ui->setupUi(this);
    ui->gridBattleEngrave->setAlignment(Qt::AlignTop);
    ui->gridClassEngrave->setAlignment(Qt::AlignTop);

    pLayout->addWidget(this);
    this->hide();

    m_layouts = {ui->gridBattleEngrave, ui->gridClassEngrave};

    initializeUI();
}

SmartSearchEngraveBook::~SmartSearchEngraveBook()
{
    for (QWidget* pWidget : m_widgets)
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
        m_layouts[i]->addWidget(pLabelCategory, 0, 0, 1, -1, Qt::AlignHCenter);
        m_widgets.append(pLabelCategory);

        for (int j = 0; j < attributes.size(); j++)
        {
            QLabel *pLabelAttribute = WidgetManager::createLabel(attributes[j], 12);
            m_layouts[i]->addWidget(pLabelAttribute, 1, j);
            m_widgets.append(pLabelAttribute);
        }
    }
}

void SmartSearchEngraveBook::updateUI()
{
    clearUI();

    const QList<QStringList*> engraveKeys = {&m_battleEngraveKeys, &m_classEngraveKeys};

    for (int i = 0; i < engraveKeys.size(); i++)
    {
        int row = 2;
        const QStringList *keys = engraveKeys[i];

        for (const QString& key : *keys)
        {
            const int &recentPrice = m_engravePrices[key].first;
            const int &minPrice = m_engravePrices[key].second;

            QFrame *pHLine = WidgetManager::createLine(QFrame::HLine);
            m_layouts[i]->addWidget(pHLine, row++, 0, 1, -1);
            m_priceWidgets.append(pHLine);

            QLabel *pIcon = WidgetManager::createIcon(":/image/item/book/0.png", nullptr);
            m_layouts[i]->addWidget(pIcon, row, 0);
            m_layouts[i]->setAlignment(pIcon, Qt::AlignHCenter);
            m_priceWidgets.append(pIcon);

            QLabel *pLabelName = WidgetManager::createLabel(key, 10, itemGradeToTextColor(ItemGrade::전설));
            m_layouts[i]->addWidget(pLabelName, row, 1);
            m_priceWidgets.append(pLabelName);

            QLabel *pLabelRecentPrice = WidgetManager::createLabel(QString("%L1").arg(recentPrice), 10);
            m_layouts[i]->addWidget(pLabelRecentPrice, row, 2);
            m_priceWidgets.append(pLabelRecentPrice);

            QLabel *pLabelMinPrice = WidgetManager::createLabel(QString("%L1").arg(minPrice), 10);
            m_layouts[i]->addWidget(pLabelMinPrice, row++, 3);
            m_priceWidgets.append(pLabelMinPrice);
        }
    }

    m_battleEngraveKeys.clear();
    m_classEngraveKeys.clear();
}

void SmartSearchEngraveBook::clearUI()
{
    for (QWidget* pWidget : m_priceWidgets)
        delete pWidget;
    m_priceWidgets.clear();
}

void SmartSearchEngraveBook::searchEngraveBook(bool bResetPageNo)
{
    if (bResetPageNo)
        m_searchPageNo = 1;

    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished, this, &SmartSearchEngraveBook::parseSearchResult);
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    SearchOption searchOption(SearchType::Market);
    searchOption.setCategoryCode(CategoryCode::EngraveBook);
    searchOption.setItemGrade(ItemGrade::전설);
    searchOption.setPageNo(m_searchPageNo);
    searchOption.setSortCondition("DESC");

    ApiManager::getInstance()->post(pNetworkManager, LostarkApi::Market, QJsonDocument(searchOption.toJsonObject()).toJson());
}

void SmartSearchEngraveBook::parseSearchResult(QNetworkReply *pReply)
{
    QJsonObject result = QJsonDocument::fromJson(pReply->readAll()).object();

    // 검색 결과 parsing
    const QJsonArray &items = result.find("Items")->toArray();
    for (const QJsonValue& value : items)
    {
        const QJsonObject &item = value.toObject();
        const QString &name = item.find("Name")->toString();
        const int &recentPrice = item.find("RecentPrice")->toInt();
        const int &minPrice = item.find("CurrentMinPrice")->toInt();

        if (name[0] == '[')
            m_classEngraveKeys << name;
        else
            m_battleEngraveKeys << name;

        m_engravePrices[name] = {recentPrice, minPrice};
    }

    const int &maxPageSize = result.find("PageSize")->toInt();

    // 다음 페이지가 있다면 추가 검색
    if (maxPageSize > m_searchPageNo)
    {
        m_searchPageNo++;
        searchEngraveBook(false);
    }
    // 마지막 페이지이면 검색 결과 ui에 반영
    else
    {
        updateUI();
    }
}
