#include "reforge_quotation.h"
#include "ui_reforge_quotation.h"
#include "ui/widget_manager.h"
#include "function/quotation/reforge/reforge_item.h"
#include "api/api_manager.h"
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
    ui->vLayoutReforgeQuotation->setAlignment(Qt::AlignHCenter);

    loadReforgeData();
    addRefreshButton();
    addReforgeItemWidgets();
}

ReforgeQuotation::~ReforgeQuotation()
{
    delete ui;
}

void ReforgeQuotation::loadReforgeData()
{
    QFile reforgeJson(":/json/json/reforge.json");
    if (!reforgeJson.open(QIODevice::ReadOnly))
    {
        qDebug() << Q_FUNC_INFO << "File open fail";
        return;
    }

    // json parsing, 멤버 변수에 할당(초기화)
    QJsonArray jArrReforge = QJsonDocument::fromJson(reforgeJson.readAll()).array();
    for (int i = 0; i < jArrReforge.size(); i++)
    {
        const QJsonObject& jObjReforge = jArrReforge[i].toObject();
        m_categories.append(jObjReforge.find("Category")->toString());

        const QJsonArray& jArrItem = jObjReforge.find("Items")->toArray();
        QList<Item> items;
        for (int j = 0; j < jArrItem.size(); j++)
        {
            const QJsonObject& jObjItem = jArrItem[j].toObject();
            Item reforgeItem(ItemType::Size);
            reforgeItem.setName(jObjItem.find("Name")->toString());
            reforgeItem.setGrade(strToItemGrade(jObjItem.find("Grade")->toString()));
            items.append(reforgeItem);
        }
        m_reforgeItems.append(items);
    }

    reforgeJson.close();
}

void ReforgeQuotation::addRefreshButton()
{
    m_pBtnRefresh = WidgetManager::createPushButton("", 14);
    m_pBtnRefresh->setIcon(QPixmap(":/icon/image/refresh.png"));
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
            ReforgeItem* pReforgeItemWidget = new ReforgeItem(reforgeItem.getName(), iconPath.arg(i).arg(j), reforgeItem.getGrade());
            reforgeItemWidgets.append(pReforgeItemWidget);
            pGroupHLayout->addWidget(pReforgeItemWidget);
        }
        m_reforgeItemWidgets.append(reforgeItemWidgets);
    }
}

void ReforgeQuotation::refreshPrice()
{
    for (int i = 0; i < m_reforgeItems.size(); i++)
    {
        for (int j = 0; j < m_reforgeItems[i].size(); j++)
        {
            const Item& reforgeItem = m_reforgeItems[i][j];
            sendRequest(m_reforgeItemWidgets[i][j], reforgeItem.getName());
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

void ReforgeQuotation::sendRequest(ReforgeItem* pReforgeItemWidget, QString itemName)
{
    QNetworkAccessManager* pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&, pReforgeItemWidget](QNetworkReply* pReply){
        QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
        if (response.isNull())
            return;
        QJsonArray items = response.object().find("Items")->toArray();
        if (items.size() == 0)
            return;

        // update price
        int price = items[0].toObject().find("CurrentMinPrice")->toInt();
        pReforgeItemWidget->setPrice(price);
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
