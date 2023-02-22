#include "raidreward_profit.h"
#include "ui_raidreward_profit.h"
#include "ui/widget_manager.h"
#include "function/raid/raidreward.h"
#include "api/api_manager.h"
#include "resource/resource_manager.h"

#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>

RaidRewardProfit* RaidRewardProfit::m_pInstance = nullptr;

RaidRewardProfit::RaidRewardProfit() :
    ui(new Ui::RaidRewardProfit)
{
    ui->setupUi(this);

    setAlignments();
    loadRewardData();
    initIconPath();
    addRefresh();
    addRaidSelector();
    addInfo();
    addRaidRewardWidgets();
}

RaidRewardProfit::~RaidRewardProfit()
{
    for (QWidget* pWidget : m_widgets)
        delete pWidget;
    for (RaidReward* pRaidRewardWidget : m_raidRewardWidgets)
        delete pRaidRewardWidget;
    delete ui;
}

void RaidRewardProfit::setAlignments()
{
    ui->vLayoutRaidRewardProfit->setAlignment(Qt::AlignHCenter);
    ui->hLayoutInput->setAlignment(Qt::AlignHCenter);
}

void RaidRewardProfit::loadRewardData()
{
    const QJsonObject json = ResourceManager::getInstance()->loadJson("raid_reward_more");

    const QJsonArray& arrRaidReward = json.find("RaidRewardMore")->toArray();
    for (int i = 0; i < arrRaidReward.size(); i++)
    {
        const QJsonObject& objRaidReward = arrRaidReward[i].toObject();

        m_raids << objRaidReward.find("Category")->toString();

        const QJsonArray& arrReward = objRaidReward.find("Rewards")->toArray();
        QList<Reward> rewards;
        for (int j = 0; j < arrReward.size(); j++)
        {
            const QJsonObject& objReward = arrReward[j].toObject();

            QString gate = objReward.find("Gate")->toString();
            int cost = objReward.find("Cost")->toInt();

            const QJsonArray& arrItem = objReward.find("Items")->toArray();
            QList<QPair<QString, int>> items;
            for (int k = 0; k < arrItem.size(); k++)
            {
                const QJsonObject& objItem = arrItem[k].toObject();

                QString name = objItem.find("Name")->toString();
                int count = objItem.find("Count")->toInt();
                items.append({name, count});
            }
            rewards.append({gate, cost, items});
        }
        m_rewards.append(rewards);
    }
}

void RaidRewardProfit::initIconPath()
{
    const QJsonObject json = ResourceManager::getInstance()->loadJson("reforge");

    QString iconPath = ":/reforge/image/reforge/reforge_%1_%2.png";
    const QJsonArray& arrReforge = json.find("Reforge")->toArray();
    for (int i = 0; i < arrReforge.size(); i++)
    {
        const QJsonArray& arrItems = arrReforge[i].toObject().find("Items")->toArray();
        for (int j = 0; j < arrItems.size(); j++)
        {
            const QJsonObject& objItem = arrItems[j].toObject();
            m_iconPaths[objItem.find("Name")->toString()] = iconPath.arg(i).arg(j);
        }
    }
    m_iconPaths["선명한 지혜의 기운"] = ":/item/image/item/vigor_0.png";
    m_iconPaths["빛나는 지혜의 기운"] = ":/item/image/item/vigor_1.png";
}

void RaidRewardProfit::addRefresh()
{
    QPushButton* pButtonRefresh = WidgetManager::createPushButton(QPixmap(":/icon/image/refresh.png"));
    m_widgets.append(pButtonRefresh);
    ui->hLayoutInput->addWidget(pButtonRefresh);
    connect(pButtonRefresh, &QPushButton::released, this, &RaidRewardProfit::updatePrice);
}

void RaidRewardProfit::addRaidSelector()
{
    QComboBox* pRaidSelector = WidgetManager::createComboBox(m_raids, 10, 150, 25);
    m_widgets.append(pRaidSelector);
    ui->hLayoutInput->addWidget(pRaidSelector);
    connect(pRaidSelector, &QComboBox::currentIndexChanged, this, [&](int index){
        for (RaidReward* pRaidRewardWidget : m_raidRewardWidgets)
            pRaidRewardWidget->hide();
        m_raidRewardWidgets[index]->show();
    });
}

void RaidRewardProfit::addInfo()
{
    QString infoText = "수호석의 경우 거래소 가격 기준이므로 실제 가격은 더 낮을 수 있음";
    QLabel* pLabelInfo = WidgetManager::createLabel(infoText, 10, "", 500, 50);
    m_widgets.append(pLabelInfo);
    ui->vLayoutRaidRewardProfit->addWidget(pLabelInfo);
    ui->vLayoutRaidRewardProfit->setAlignment(pLabelInfo, Qt::AlignHCenter);
}

void RaidRewardProfit::addRaidRewardWidgets()
{
    for (int i = 0; i < m_raids.size(); i++)
    {
        RaidReward* pRaidRewardWidget = new RaidReward(m_raids[i], m_rewards[i], m_iconPaths);
        m_raidRewardWidgets.append(pRaidRewardWidget);
        ui->vLayoutRaidRewardProfit->addWidget(pRaidRewardWidget);
        pRaidRewardWidget->hide();
    }
    m_raidRewardWidgets[0]->show();
}

void RaidRewardProfit::updatePrice()
{
    QStringList items = {"명예의 파편 주머니(소)",
                         "파괴석 결정", "파괴강석", "정제된 파괴강석",
                         "수호석 결정", "수호강석", "정제된 수호강석",
                         "위대한 명예의 돌파석", "경이로운 명예의 돌파석", "찬란한 명예의 돌파석",
                         "선명한 지혜의 정수", "빛나는 지혜의 정수"};

    for (const QString& item : items)
    {
        QJsonObject searchOption = ApiManager::getInstance()->buildSearchOption(SearchType::Market, CategoryCode::Reforge, item);
        QNetworkAccessManager* pNetworkManager = new QNetworkAccessManager();
        connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&, item](QNetworkReply* pReply){
            QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
            if (response.isNull())
                return;

            const QJsonArray& jArrItems = response.object().find("Items")->toArray();
            if (jArrItems.size() == 0)
                return;

            int price = jArrItems[0].toObject().find("CurrentMinPrice")->toInt();
            if (item.contains("명예의 파편"))
                m_itemPrices["명예의 파편"] = price / (double)500;
            else if (item.contains("파괴") || item.contains("수호"))
                m_itemPrices[item] = price / (double)10;
            else if (item == "선명한 지혜의 정수")
                m_itemPrices["선명한 지혜의 기운"] = price * (65 / (double)4);
            else if (item == "빛나는 지혜의 정수")
                m_itemPrices["빛나는 지혜의 기운"] = price * (75 / (double)4);
            else
                m_itemPrices[item] = price;

            for (RaidReward* pRaidRewardWidget : m_raidRewardWidgets)
            {
                if (item.contains("명예의 파편"))
                    pRaidRewardWidget->updateItemPrice("명예의 파편");
                else if (item == "선명한 지혜의 정수")
                    pRaidRewardWidget->updateItemPrice("선명한 지혜의 기운");
                else if (item == "빛나는 지혜의 정수")
                    pRaidRewardWidget->updateItemPrice("빛나는 지혜의 기운");
                else
                    pRaidRewardWidget->updateItemPrice(item);
            }
        });
        connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);
        ApiManager::getInstance()->post(pNetworkManager, LostarkApi::Market, QJsonDocument(searchOption).toJson());
    }
}

RaidRewardProfit* RaidRewardProfit::getInstance()
{
    if (m_pInstance == nullptr)
        m_pInstance = new RaidRewardProfit();
    return m_pInstance;
}

void RaidRewardProfit::destroyInstance()
{
    if (m_pInstance == nullptr)
        return;
    delete m_pInstance;
    m_pInstance = nullptr;
}

double RaidRewardProfit::getItemPrice(QString item)
{
    return m_itemPrices[item];
}
