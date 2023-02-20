#include "reforge_quotation.h"
#include "ui_reforge_quotation.h"
#include "ui/widget_manager.h"
#include "function/quotation/reforge/reforge_item.h"
#include "api/api_manager.h"
#include "resource/resource_manager.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <QNetworkReply>

ReforgeQuotation* ReforgeQuotation::m_pInstance = nullptr;

ReforgeQuotation::ReforgeQuotation() :
    ui(new Ui::ReforgeQuotation)
{
    ui->setupUi(this);
    ui->vLayoutReforgeQuotation->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    ui->vLayoutEfficiency->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    loadReforgeData();
    addRefreshButton();
    addReforgeItemWidgets();
    addEfficiencyWidgets();
}

ReforgeQuotation::~ReforgeQuotation()
{
    delete ui;
}

void ReforgeQuotation::loadReforgeData()
{
    const QJsonObject json = ResourceManager::getInstance()->loadJson("reforge");

    // json parsing, 멤버 변수에 할당(초기화)
    QJsonArray arrReforge = json.find("Reforge")->toArray();
    for (int i = 0; i < arrReforge.size(); i++)
    {
        const QJsonObject& objReforge = arrReforge[i].toObject();
        m_categories.append(objReforge.find("Category")->toString());

        const QJsonArray& arrItems = objReforge.find("Items")->toArray();
        QList<Item> items;
        for (int j = 0; j < arrItems.size(); j++)
        {
            const QJsonObject& objItem = arrItems[j].toObject();
            Item reforgeItem(ItemType::Size);
            reforgeItem.setName(objItem.find("Name")->toString());
            reforgeItem.setGrade(strToItemGrade(objItem.find("Grade")->toString()));
            // "명예의 파편"은 제외
            if (reforgeItem.getName() == "명예의 파편")
                continue;
            items.append(reforgeItem);
        }
        m_reforgeItems.append(items);
    }
}

void ReforgeQuotation::addRefreshButton()
{
    m_pBtnRefresh = WidgetManager::createPushButton(QPixmap(":/icon/image/refresh.png"), 14);
    ui->vLayoutReforgeQuotation->addWidget(m_pBtnRefresh);
    ui->vLayoutReforgeQuotation->setAlignment(m_pBtnRefresh, Qt::AlignHCenter);
    connect(m_pBtnRefresh, &QPushButton::released, this, &ReforgeQuotation::refreshPrice);
}

void ReforgeQuotation::addReforgeItemWidgets()
{
    QString iconPath = ":/reforge/image/reforge/reforge_%1_%2.png";

    for (int i = 0; i < m_reforgeItems.size(); i++)
    {
        QHBoxLayout* pHLayout = new QHBoxLayout();
        ui->vLayoutReforgeQuotation->addLayout(pHLayout);
        m_layouts.append(pHLayout);

        QLabel* pLabelCategory = WidgetManager::createLabel(m_categories[i], 14);
        m_labels.append(pLabelCategory);
        pHLayout->addWidget(pLabelCategory);

        QGroupBox* pGroupBox = new QGroupBox();
        m_groupBoxes.append(pGroupBox);
        pHLayout->addWidget(pGroupBox);

        QHBoxLayout* pGroupHLayout = new QHBoxLayout();
        m_layouts.append(pGroupHLayout);
        pGroupBox->setLayout(pGroupHLayout);

        QList<ReforgeItem*> reforgeItemWidgets;
        for (int j = 0; j < m_reforgeItems[i].size(); j++)
        {
            const Item& reforgeItem = m_reforgeItems[i][j];
            // 파괴석, 수호석은 10개 단위로 거래 (아이템명에 표기)
            QString name = reforgeItem.getName();
            if (m_categories[i] == "파괴석" || m_categories[i] == "수호석")
                name += " x10";

            ReforgeItem* pReforgeItemWidget = new ReforgeItem(name, iconPath.arg(i).arg(j), reforgeItem.getGrade());
            reforgeItemWidgets.append(pReforgeItemWidget);
            pGroupHLayout->addWidget(pReforgeItemWidget);
        }
        m_reforgeItemWidgets.append(reforgeItemWidgets);
    }
}

void ReforgeQuotation::addEfficiencyWidgets()
{
    QLabel* pLabelEfficiencyTitle = WidgetManager::createLabel("[효율]", 14, "", 100, 50);
    m_labels.append(pLabelEfficiencyTitle);
    ui->vLayoutEfficiency->addWidget(pLabelEfficiencyTitle);
    ui->vLayoutEfficiency->setAlignment(pLabelEfficiencyTitle, Qt::AlignHCenter);

    // 명예의 파편 효율 Layout 추가
    {
        QGroupBox* pGroupBox = new QGroupBox();
        m_groupBoxes.append(pGroupBox);
        ui->vLayoutEfficiency->addWidget(pGroupBox);

        QVBoxLayout* pVLayout = new QVBoxLayout();
        m_layouts.append(pVLayout);
        pGroupBox->setLayout(pVLayout);

        QLabel* pLabelCategory = WidgetManager::createLabel("명예의 파편 1개당 가격", 12, "", 200, 50);
        m_labels.append(pLabelCategory);
        pVLayout->addWidget(pLabelCategory);
        pVLayout->setAlignment(pLabelCategory, Qt::AlignHCenter);

        QHBoxLayout* pHLayout = new QHBoxLayout();
        m_layouts.append(pHLayout);
        pVLayout->addLayout(pHLayout);

        QStringList itemNames = {"(소)", "(중)", "(대)"};
        QList<ItemGrade> itemGrades = {ItemGrade::고급, ItemGrade::희귀, ItemGrade::영웅};
        QList<QLabel*> efficiencyLabels;
        for (int i = 0; i < itemNames.size(); i++)
        {
            QLabel* pLabelItemName = WidgetManager::createLabel(itemNames[i], 10, colorCode(itemGrades[i]), 50);
            m_labels.append(pLabelItemName);
            pHLayout->addWidget(pLabelItemName);

            QLabel* pLabelEfficiency = WidgetManager::createLabel("[-]", 10, "", 100);
            pLabelEfficiency->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            efficiencyLabels.append(pLabelEfficiency);
            pHLayout->addWidget(pLabelEfficiency);
        }
        m_efficiencyLabels.append(efficiencyLabels);
    }
}

void ReforgeQuotation::refreshPrice()
{
    for (int i = 0; i < m_reforgeItems.size(); i++)
    {
        for (int j = 0; j < m_reforgeItems[i].size(); j++)
        {
            const Item& reforgeItem = m_reforgeItems[i][j];
            if (i == 0)
                sendRequest(m_reforgeItemWidgets[i][j], m_efficiencyLabels[i][j], reforgeItem.getName());
            else
                sendRequest(m_reforgeItemWidgets[i][j], nullptr, reforgeItem.getName());
        }
    }
}

void ReforgeQuotation::refreshEfficiency(QLabel* pLabelEfficiency, QString itemName, int price)
{
    // 명예의 파편 소, 중, 대 효율 갱신
    {
        QStringList contains = {"(소)", "(중)", "(대)"};
        QList<double> itemVolumes = {500, 1000, 1500};
        for (int i = 0; i < contains.size(); i++)
        {
            if (itemName.contains(contains[i]))
            {
                double pricePerPiece = price / itemVolumes[i];
                pLabelEfficiency->setText(QString("[%L1골]").arg(pricePerPiece, 0, 'f', 3, QChar(' ')));
            }
        }
    }
}

QJsonObject ReforgeQuotation::buildSearchOption(QString itemName)
{
    QJsonObject searchOption;

    searchOption.insert("Sort", "CURRENT_MIN_PRICE");
    searchOption.insert("CategoryCode", 50000);
    searchOption.insert("ItemTier", 3);
    searchOption.insert("ItemName", itemName);
    searchOption.insert("PageNo", 1);
    searchOption.insert("SortCondition", "ASC");

    return searchOption;
}

void ReforgeQuotation::sendRequest(ReforgeItem* pReforgeItemWidget, QLabel* pLabelEfficiency, QString itemName)
{
    QNetworkAccessManager* pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&, pReforgeItemWidget, pLabelEfficiency, itemName](QNetworkReply* pReply){
        QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
        if (response.isNull())
            return;
        QJsonArray items = response.object().find("Items")->toArray();
        if (items.size() == 0)
            return;

        // update price
        int price = items[0].toObject().find("CurrentMinPrice")->toInt();
        pReforgeItemWidget->setPrice(price);
        // update efficiency
        if (pLabelEfficiency != nullptr)
            refreshEfficiency(pLabelEfficiency, itemName, price);
    });
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    QByteArray data = QJsonDocument(buildSearchOption(itemName)).toJson();
    ApiManager::getInstance()->post(pNetworkManager, LostarkApi::Market, data);
}

ReforgeQuotation* ReforgeQuotation::getInstance()
{
    if (m_pInstance == nullptr)
        m_pInstance = new ReforgeQuotation();
    return m_pInstance;
}

void ReforgeQuotation::destroyInstance()
{
    if (m_pInstance == nullptr)
        return;
    delete m_pInstance;
    m_pInstance = nullptr;
}
