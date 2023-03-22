#include "smart_search_reforge.h"
#include "ui_smart_search_reforge.h"
#include "ui/widget_manager.h"
#include "resource/resource_manager.h"
#include "api/response_parser.h"
#include "api/api_manager.h"
#include "api/search_option.h"

#include <QLabel>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>

SmartSearchReforge::SmartSearchReforge(QLayout *pLayout) :
    SmartSearchMenu(),
    ui(new Ui::SmartSearchReforge)
{
    ui->setupUi(this);

    pLayout->addWidget(this);
    this->hide();

    loadResource();
    initializeUI();
}

SmartSearchReforge::~SmartSearchReforge()
{
    for (QLabel* pLabel : mYDayAvgPriceLabels)
        delete pLabel;
    for (QLabel* pLabel : mRecentPriceLabels)
        delete pLabel;
    for (QLabel* pLabel : mMinPriceLabels)
        delete pLabel;
    for (QLabel* pLabel : mEfficiencyLabels)
        delete pLabel;
    for (QWidget* pWidget : mWidgets)
        delete pWidget;
    for (auto rIter = mLayouts.rbegin(); rIter != mLayouts.rend(); rIter++)
        delete *rIter;
    mLayouts.clear();
    delete ui;
}

void SmartSearchReforge::refresh()
{
    int labelIndex = 0;

    for (int i = 0; i < mCategories.size(); i++)
    {
        for (int j = 0; j < mItems[i].size(); j++)
        {
            const Item &item = mItems[i][j];

            QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
            connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&, labelIndex](QNetworkReply *pReply){
                QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
                if (response.isNull())
                    return;

                ResponseMarket responseMarket = ResponseParser::parseMarketItem(response);
                const MarketItem &item = responseMarket.items.front();

                // 가격 정보 업데이트
                mYDayAvgPriceLabels[labelIndex]->setText(QString("%L1").arg(item.yDayAvgPrice));
                mRecentPriceLabels[labelIndex]->setText(QString("%L1").arg(item.recentPrice));
                mMinPriceLabels[labelIndex]->setText(QString("%L1").arg(item.currentMinPrice));

                // 개당 가격 업데이트
                if (labelIndex < 3)
                {
                    const int volumes[3] = {500, 1000, 1500};
                    double efficiency = item.currentMinPrice / (double)volumes[labelIndex];

                    mEfficiencyLabels[labelIndex]->setText(QString::number(efficiency, 'f', 3));
                }
            });
            connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

            // option setting 후 api 요청(post) 전달
            SearchOption searchOption(SearchType::Market);
            searchOption.setCategoryCode(CategoryCode::Reforge);
            searchOption.setItemName(item.itemName());
            searchOption.setItemTier(3);
            searchOption.setPageNo(1);
            searchOption.setSortCondition("ASC");

            ApiManager::getInstance()->post(pNetworkManager, LostarkApi::Market, QJsonDocument(searchOption.toJsonObject()).toJson());

            labelIndex++;
        }
    }
}

void SmartSearchReforge::loadResource()
{
    QJsonObject json = ResourceManager::getInstance()->loadJson("reforge");
    const QString iconPath = ":/image/item/reforge/%1_%2.png";

    const QJsonArray &reforges = json.find("Reforge")->toArray();
    for (int i = 0; i < reforges.size(); i++)
    {
        const QJsonObject &reforge = reforges[i].toObject();

        // category 추가
        mCategories << reforge.find("Category")->toString();

        // item 목록 추가
        QList<Item> items;
        const QJsonArray &jsonItems = reforge.find("Items")->toArray();
        for (int j = 0; j < jsonItems.size(); j++)
        {
            const QJsonObject &object = jsonItems[j].toObject();
            const QString &name = object.find("Name")->toString();
            if (name == "명예의 파편")
                continue;

            Item item(ItemType::Default);
            item.setItemName(name);
            item.setItemGrade(qStringToItemGrade(object.find("Grade")->toString()));
            item.setIconPath(iconPath.arg(i).arg(j));
            items.append(item);
        }
        mItems.append(items);
    }
}

void SmartSearchReforge::initializeUI()
{
    // 속성 label 추가
    const QStringList attributes = {"#", "재료", "전일 평균 거래가", "최근 거래가", "현재 최저가", "개당 가격"};

    for (int col = 0; col < attributes.size(); col++)
    {
        QLabel *pLabelAttribute = WidgetManager::createLabel(attributes[col], 14);
        ui->gridReforge->addWidget(pLabelAttribute, 0, col);
        mWidgets.append(pLabelAttribute);
    }

    // 아이템 목록 추가
    int row = 1;

    for (int i = 0; i < mCategories.size(); i++)
    {
        for (const Item& item : mItems[i])
        {
            int col = 0;

            QFrame *pHLine = WidgetManager::createLine(QFrame::HLine);
            ui->gridReforge->addWidget(pHLine, row++, 0, 1, -1);
            mWidgets.append(pHLine);

            QLabel *pIcon = WidgetManager::createIcon(item.iconPath(), nullptr, itemGradeToBGColor(item.itemGrade()));
            ui->gridReforge->addWidget(pIcon, row, col++);
            mWidgets.append(pIcon);

            // 아이템명
            QString itemName = "";

            if (mCategories[i] == "파괴석" || mCategories[i] == "수호석")
                itemName = item.itemName() + " x10";
            else
                itemName = item.itemName();

            QLabel *pLabelName = WidgetManager::createLabel(itemName, 10, itemGradeToTextColor(item.itemGrade()));
            ui->gridReforge->addWidget(pLabelName, row, col++);
            mWidgets.append(pLabelName);

            // 전일 평균 거래가
            QLabel *pLabelYDayAvgPrice = WidgetManager::createLabel("-");
            ui->gridReforge->addWidget(pLabelYDayAvgPrice, row, col++);
            mYDayAvgPriceLabels.append(pLabelYDayAvgPrice);

            // 최근 거래가
            QLabel *pLabelRecentPrice = WidgetManager::createLabel("-");
            ui->gridReforge->addWidget(pLabelRecentPrice, row, col++);
            mRecentPriceLabels.append(pLabelRecentPrice);

            // 현재 최저가
            QLabel *pLabelMinPrice = WidgetManager::createLabel("-");
            ui->gridReforge->addWidget(pLabelMinPrice, row, col++);
            mMinPriceLabels.append(pLabelMinPrice);

            // 개당 가격
            QLabel *pLabelEfficiency = WidgetManager::createLabel("-");
            ui->gridReforge->addWidget(pLabelEfficiency, row, col++);
            mEfficiencyLabels.append(pLabelEfficiency);

            row++;
        }
    }
}
