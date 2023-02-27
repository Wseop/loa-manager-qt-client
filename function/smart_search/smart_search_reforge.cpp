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
    initUI();
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

void SmartSearchReforge::updatePrice(bool bResetPageNo)
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
            searchOption.setItemName(item.getName());
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
    const QString iconPath = ":/reforge/image/reforge/reforge_%1_%2.png";

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

            Item item(ItemType::Size);
            item.setName(name);
            item.setGrade(strToItemGrade(object.find("Grade")->toString()));
            item.setIconPath(iconPath.arg(i).arg(j));
            items.append(item);
        }
        m_items.append(items);
    }
}

void SmartSearchReforge::initUI()
{
    const int LABEL_WIDTH = 150;

    // 속성 label 추가
    const QStringList attributes = {"#", "재료", "최근 거래가", "현재 최저가", "효율 (개당 가격)"};
    for (int col = 0; col < attributes.size(); col++)
    {
        QLabel* pLabelAttribute = nullptr;
        if (col == 0)
            pLabelAttribute = WidgetManager::createLabel(attributes[col], 14, "", 50);
        else
            pLabelAttribute = WidgetManager::createLabel(attributes[col], 14, "", LABEL_WIDTH);
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

            QLabel* pIcon = WidgetManager::createIcon(item.getIconPath(), nullptr, backgroundColorCode(item.getGrade()));
            ui->gridReforge->addWidget(pIcon, row, col++);
            m_widgets.append(pIcon);

            QLabel* pLabelName = nullptr;
            if (m_categories[i] == "파괴석" || m_categories[i] == "수호석")
                pLabelName = WidgetManager::createLabel(item.getName() + " x10", 10, colorCode(item.getGrade()), LABEL_WIDTH);
            else
                pLabelName = WidgetManager::createLabel(item.getName(), 10, colorCode(item.getGrade()), LABEL_WIDTH);
            ui->gridReforge->addWidget(pLabelName, row, col++);
            m_widgets.append(pLabelName);

            QLabel* pLabelRecentPrice = WidgetManager::createLabel("-", 10, "", LABEL_WIDTH);
            ui->gridReforge->addWidget(pLabelRecentPrice, row, col++);
            m_recentPriceLabels.append(pLabelRecentPrice);

            QLabel* pLabelMinPrice = WidgetManager::createLabel("-", 10, "", LABEL_WIDTH);
            ui->gridReforge->addWidget(pLabelMinPrice, row, col++);
            m_minPriceLabels.append(pLabelMinPrice);

            QLabel* pLabelEfficiency = WidgetManager::createLabel("-", 10, "", LABEL_WIDTH);
            ui->gridReforge->addWidget(pLabelEfficiency, row, col++);
            m_efficiencyLabels.append(pLabelEfficiency);

            row++;
        }
    }
}
