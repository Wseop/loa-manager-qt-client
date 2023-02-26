#include "reforge_quotation.h"
#include "ui_reforge_quotation.h"
#include "ui/widget_manager.h"
#include "function/quotation/reforge/reforge_item.h"
#include "api/api_manager.h"
#include "api/search_option.h"
#include "resource/resource_manager.h"

#include <QFile>
#include <QJsonObject>
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
    initRefreshButton();
    initReforgeItemWidget();
    initEfficiencyWidget();
}

ReforgeQuotation::~ReforgeQuotation()
{
    for (QWidget* pWidget : m_widgets)
        delete pWidget;
    for (QLayout* pLayout : m_layouts)
        delete pLayout;
    for (int i = 0; i < m_reforgeItemWidgets.size(); i++)
        for (int j = 0; j < m_reforgeItemWidgets[i].size(); j++)
            delete m_reforgeItemWidgets[i][j];
    for (int i = 0; i < m_efficiencyLabels.size(); i++)
        for (int j = 0; j < m_efficiencyLabels[i].size(); j++)
            delete m_efficiencyLabels[i][j];
    delete ui;
}

void ReforgeQuotation::loadReforgeData()
{
    const QJsonObject json = ResourceManager::getInstance()->loadJson("reforge");

    // json parsing, 멤버 변수에 할당(초기화)
    QJsonArray reforges = json.find("Reforge")->toArray();
    for (int i = 0; i < reforges.size(); i++)
    {
        const QJsonObject& reforge = reforges[i].toObject();
        m_categories.append(reforge.find("Category")->toString());

        QList<Item> items;
        const QJsonArray& jsonItems = reforge.find("Items")->toArray();
        for (int j = 0; j < jsonItems.size(); j++)
        {
            const QJsonObject& jsonItem = jsonItems[j].toObject();

            Item item(ItemType::Size);
            item.setName(jsonItem.find("Name")->toString());
            item.setGrade(strToItemGrade(jsonItem.find("Grade")->toString()));

            // "명예의 파편"은 제외
            if (item.getName() == "명예의 파편")
                continue;
            items.append(item);
        }
        m_reforgeItems.append(items);
    }
}

void ReforgeQuotation::initRefreshButton()
{
    QPushButton* pRefreshButton = WidgetManager::createPushButton(QPixmap(":/icon/image/refresh.png"), 14);
    ui->vLayoutReforgeQuotation->addWidget(pRefreshButton);
    ui->vLayoutReforgeQuotation->setAlignment(pRefreshButton, Qt::AlignHCenter);
    connect(pRefreshButton, &QPushButton::released, this, &ReforgeQuotation::refreshPrice);
    m_widgets.append(pRefreshButton);
}

void ReforgeQuotation::initReforgeItemWidget()
{
    QString iconPath = ":/reforge/image/reforge/reforge_%1_%2.png";

    // item category별로 묶어서 출력
    for (int i = 0; i < m_reforgeItems.size(); i++)
    {
        QHBoxLayout* pHLayout = new QHBoxLayout();
        ui->vLayoutReforgeQuotation->addLayout(pHLayout);
        m_layouts.append(pHLayout);

        QLabel* pLabelCategory = WidgetManager::createLabel(m_categories[i], 14);
        pHLayout->addWidget(pLabelCategory);
        m_widgets.append(pLabelCategory);

        QGroupBox* pGroupBox = new QGroupBox();
        pHLayout->addWidget(pGroupBox);
        m_widgets.append(pGroupBox);

        QHBoxLayout* pGroupHLayout = new QHBoxLayout();
        pGroupBox->setLayout(pGroupHLayout);
        m_layouts.append(pGroupHLayout);

        QList<ReforgeItem*> reforgeItemWidgets;
        for (int j = 0; j < m_reforgeItems[i].size(); j++)
        {
            const Item& reforgeItem = m_reforgeItems[i][j];

            QString name = reforgeItem.getName();
            // 파괴석, 수호석은 10개 단위로 거래
            if (m_categories[i] == "파괴석" || m_categories[i] == "수호석")
                name += " x10";

            ReforgeItem* pReforgeItemWidget = new ReforgeItem(name, iconPath.arg(i).arg(j), reforgeItem.getGrade());
            reforgeItemWidgets.append(pReforgeItemWidget);
            pGroupHLayout->addWidget(pReforgeItemWidget);
        }
        m_reforgeItemWidgets.append(reforgeItemWidgets);
    }
}

void ReforgeQuotation::initEfficiencyWidget()
{
    QLabel* pLabelEfficiencyTitle = WidgetManager::createLabel("[효율]", 14, "", 100, 50);
    ui->vLayoutEfficiency->addWidget(pLabelEfficiencyTitle);
    ui->vLayoutEfficiency->setAlignment(pLabelEfficiencyTitle, Qt::AlignHCenter);
    m_widgets.append(pLabelEfficiencyTitle);

    // 명예의 파편 효율 Layout 추가
    QGroupBox* pGroupBox = new QGroupBox();
    ui->vLayoutEfficiency->addWidget(pGroupBox);
    m_widgets.append(pGroupBox);

    QVBoxLayout* pVLayout = new QVBoxLayout();
    pGroupBox->setLayout(pVLayout);
    m_layouts.append(pVLayout);

    QLabel* pLabelCategory = WidgetManager::createLabel("명예의 파편 1개당 가격", 12, "", 200, 50);
    pVLayout->addWidget(pLabelCategory);
    pVLayout->setAlignment(pLabelCategory, Qt::AlignHCenter);
    m_widgets.append(pLabelCategory);

    QHBoxLayout* pHLayout = new QHBoxLayout();
    pVLayout->addLayout(pHLayout);
    m_layouts.append(pHLayout);

    const QStringList itemNames = {"(소)", "(중)", "(대)"};
    const QList<ItemGrade> itemGrades = {ItemGrade::고급, ItemGrade::희귀, ItemGrade::영웅};
    QList<QLabel*> efficiencyLabels;
    for (int i = 0; i < itemNames.size(); i++)
    {
        QLabel* pLabelItemName = WidgetManager::createLabel(itemNames[i], 10, colorCode(itemGrades[i]), 50);
        pHLayout->addWidget(pLabelItemName);
        m_widgets.append(pLabelItemName);

        QLabel* pLabelEfficiency = WidgetManager::createLabel("[-]", 10, "", 100);
        pLabelEfficiency->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        pHLayout->addWidget(pLabelEfficiency);
        efficiencyLabels.append(pLabelEfficiency);
    }
    m_efficiencyLabels.append(efficiencyLabels);
}

void ReforgeQuotation::refreshPrice()
{
    // 모든 아이템 가격 정보 가져오기 (api request)
    for (int i = 0; i < m_reforgeItems.size(); i++)
    {
        for (int j = 0; j < m_reforgeItems[i].size(); j++)
        {
            const Item& reforgeItem = m_reforgeItems[i][j];

            if (i == 0)
                sendApiRequest(m_reforgeItemWidgets[i][j], m_efficiencyLabels[i][j], reforgeItem.getName());
            else
                sendApiRequest(m_reforgeItemWidgets[i][j], nullptr, reforgeItem.getName());
        }
    }
}

void ReforgeQuotation::refreshEfficiency(QLabel* pLabelEfficiency, QString itemName, int price)
{
    // 명예의 파편 소, 중, 대 효율 갱신
    const QStringList contains = {"(소)", "(중)", "(대)"};
    const QList<double> itemVolumes = {500, 1000, 1500};
    for (int i = 0; i < contains.size(); i++)
    {
        if (itemName.contains(contains[i]))
        {
            double pricePerPiece = price / itemVolumes[i];
            pLabelEfficiency->setText(QString("[%L1골]").arg(pricePerPiece, 0, 'f', 3, QChar(' ')));
        }
    }
}

void ReforgeQuotation::sendApiRequest(ReforgeItem* pReforgeItemWidget, QLabel* pLabelEfficiency, QString itemName)
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

    // build search option
    SearchOption searchOption(SearchType::Market);
    searchOption.setCategoryCode(CategoryCode::Reforge);
    searchOption.setItemTier(3);
    searchOption.setItemName(itemName);
    searchOption.setPageNo(1);
    searchOption.setSortCondition("ASC");

    ApiManager::getInstance()->post(pNetworkManager, LostarkApi::Market, QJsonDocument(searchOption.toJsonObject()).toJson());
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
