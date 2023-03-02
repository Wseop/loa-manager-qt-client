#include "smart_search_reforge.h"
#include "ui_smart_search_reforge.h"
#include "ui/widget_manager.h"
#include "resource/resource_manager.h"
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

SmartSearchReforge::SmartSearchReforge(QLayout* pLayout) :
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
    for (QLabel* pLabel : m_recentPriceLabels)
        delete pLabel;
    for (QLabel* pLabel : m_minPriceLabels)
        delete pLabel;
    for (QLabel* pLabel : m_efficiencyLabels)
        delete pLabel;
    for (QWidget* pWidget : m_widgets)
        delete pWidget;
    for (QLayout* pLayout : m_layouts)
        delete pLayout;
    delete ui;
}

void SmartSearchReforge::refresh()
{
    int labelIndex = 0;

    for (int i = 0; i < m_categories.size(); i++)
    {
        for (int j = 0; j < m_items[i].size(); j++)
        {
            const Item& item = m_items[i][j];

            QNetworkAccessManager* pNetworkManager = new QNetworkAccessManager();
            connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&, labelIndex](QNetworkReply* pReply){
                // 데이터 parsing
                QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
                if (response.isNull())
                    return;

                const QJsonObject& item = response.object().find("Items")->toArray()[0].toObject();
                const QString& name = item.find("Name")->toString();
                const int& recentPrice = item.find("RecentPrice")->toInt();
                const int& minPrice = item.find("CurrentMinPrice")->toInt();

                // 가격 정보 업데이트
                m_recentPriceLabels[labelIndex]->setText(QString("%L1").arg(recentPrice));
                m_minPriceLabels[labelIndex]->setText(QString("%L1").arg(minPrice));

                // 효율 업데이트 (명예의 파편)
                if (labelIndex < 3)
                {
                    const int volumes[3] = {500, 1000, 1500};
                    double efficiency = minPrice / (double)volumes[labelIndex];

                    m_efficiencyLabels[labelIndex]->setText(QString::number(efficiency, 'f', 3));
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

    const QJsonArray& reforges = json.find("Reforge")->toArray();
    for (int i = 0; i < reforges.size(); i++)
    {
        const QJsonObject& reforge = reforges[i].toObject();

        // category 추가
        m_categories << reforge.find("Category")->toString();

        // item 목록 추가
        QList<Item> items;
        const QJsonArray& jsonItems = reforge.find("Items")->toArray();
        for (int j = 0; j < jsonItems.size(); j++)
        {
            const QJsonObject& object = jsonItems[j].toObject();
            const QString& name = object.find("Name")->toString();
            if (name == "명예의 파편")
                continue;

            Item item(ItemType::Default);
            item.setItemName(name);
            item.setItemGrade(qStringToItemGrade(object.find("Grade")->toString()));
            item.setIconPath(iconPath.arg(i).arg(j));
            items.append(item);
        }
        m_items.append(items);
    }
}

void SmartSearchReforge::initializeUI()
{
    // 속성 label 추가
    const QStringList attributes = {"#", "재료", "최근 거래가", "현재 최저가", "효율 (개당 가격)"};
    for (int col = 0; col < attributes.size(); col++)
    {
        QLabel* pLabelAttribute = WidgetManager::createLabel(attributes[col], 14);
        ui->gridReforge->addWidget(pLabelAttribute, 0, col);
        m_widgets.append(pLabelAttribute);
    }

    // 아이템 목록 추가
    int row = 1;
    for (int i = 0; i < m_categories.size(); i++)
    {
        for (const Item& item : m_items[i])
        {
            int col = 0;

            QFrame* pHLine = WidgetManager::createLine(QFrame::HLine);
            ui->gridReforge->addWidget(pHLine, row++, 0, 1, -1);
            m_widgets.append(pHLine);

            QLabel* pIcon = WidgetManager::createIcon(item.iconPath(), nullptr, itemGradeToBGColor(item.itemGrade()));
            ui->gridReforge->addWidget(pIcon, row, col++);
            m_widgets.append(pIcon);

            QString itemName = "";
            if (m_categories[i] == "파괴석" || m_categories[i] == "수호석")
                itemName = item.itemName() + " x10";
            else
                itemName = item.itemName();
            QLabel* pLabelName = WidgetManager::createLabel(itemName, 10, itemGradeToTextColor(item.itemGrade()));
            ui->gridReforge->addWidget(pLabelName, row, col++);
            m_widgets.append(pLabelName);

            QLabel* pLabelRecentPrice = WidgetManager::createLabel("-", 10);
            ui->gridReforge->addWidget(pLabelRecentPrice, row, col++);
            m_recentPriceLabels.append(pLabelRecentPrice);

            QLabel* pLabelMinPrice = WidgetManager::createLabel("-", 10);
            ui->gridReforge->addWidget(pLabelMinPrice, row, col++);
            m_minPriceLabels.append(pLabelMinPrice);

            QLabel* pLabelEfficiency = WidgetManager::createLabel("-", 10);
            ui->gridReforge->addWidget(pLabelEfficiency, row, col++);
            m_efficiencyLabels.append(pLabelEfficiency);

            row++;
        }
    }
}
