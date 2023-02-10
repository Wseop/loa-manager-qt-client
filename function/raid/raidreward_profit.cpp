#include "raidreward_profit.h"
#include "ui_raidreward_profit.h"
#include "ui/widget_manager.h"
#include "function/raid/raidreward.h"
#include "api/api_manager.h"
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
    QFile file(":/json/json/raid_reward_more.json");
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << Q_FUNC_INFO << "File open fail";
        return;
    }

    QJsonArray jArrRaidReward = QJsonDocument::fromJson(file.readAll()).array();
    for (int i = 0; i < jArrRaidReward.size(); i++)
    {
        const QJsonObject& jObjRaidReward = jArrRaidReward[i].toObject();
        const QJsonArray& jArrReward = jObjRaidReward.find("Rewards")->toArray();

        m_raids << jObjRaidReward.find("Category")->toString();
        QList<Reward> rewards;
        for (int j = 0; j < jArrReward.size(); j++)
        {
            const QJsonObject& jObjReward = jArrReward[j].toObject();

            QString gate = jObjReward.find("Gate")->toString();
            int cost = jObjReward.find("Cost")->toInt();
            QList<QPair<QString, int>> items;
            const QJsonArray& jArrItem = jObjReward.find("Items")->toArray();
            for (int k = 0; k < jArrItem.size(); k++)
            {
                const QJsonObject& jObjItem = jArrItem[k].toObject();

                QString name = jObjItem.find("Name")->toString();
                int count = jObjItem.find("Count")->toInt();
                items.append({name, count});
            }
            rewards.append({gate, cost, items});
        }
        m_rewards.append(rewards);
    }

    file.close();
}

void RaidRewardProfit::initIconPath()
{
    QFile file(":/json/json/reforge.json");
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << Q_FUNC_INFO << "File open fail";
        return;
    }

    QString iconPath = ":/reforge/image/reforge/reforge_%1_%2.png";
    QJsonArray jArrReforge = QJsonDocument::fromJson(file.readAll()).array();
    for (int i = 0; i < jArrReforge.size(); i++)
    {
        const QJsonArray& jArrItems = jArrReforge[i].toObject().find("Items")->toArray();
        for (int j = 0; j < jArrItems.size(); j++)
        {
            const QJsonObject& jObjItem = jArrItems[j].toObject();
            m_iconPaths[jObjItem.find("Name")->toString()] = iconPath.arg(i).arg(j);
        }
    }

    file.close();
}

void RaidRewardProfit::addRefresh()
{
    QPushButton* pButtonRefresh = WidgetManager::createPushButton("");
    pButtonRefresh->setIcon(QPixmap(":/icon/image/refresh.png"));
    m_widgets.append(pButtonRefresh);
    ui->hLayoutInput->addWidget(pButtonRefresh);
    connect(pButtonRefresh, &QPushButton::released, this, &RaidRewardProfit::updatePrice);
}

void RaidRewardProfit::addRaidSelector()
{
    QComboBox* pRaidSelector = new QComboBox();
    pRaidSelector->addItems(m_raids);
    pRaidSelector->setFixedSize(150, 25);
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
    QString infoText = "수호석의 경우 거래소 가격 기준이므로\n실제 가격은 더 낮을 수 있습니다.";
    QLabel* pLabelInfo = WidgetManager::createLabel(infoText, 10, "", 250, 50);
    m_widgets.append(pLabelInfo);
    ui->hLayoutInput->addWidget(pLabelInfo);
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
                         "위대한 명예의 돌파석", "경이로운 명예의 돌파석", "찬란한 명예의 돌파석"};

    for (const QString& item : items)
    {
        QJsonObject searchOption = buildSearchOption(item);
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
            {
                m_itemPrices["명예의 파편"] = price / (double)500;
            }
            else if (item.contains("파괴") || item.contains("수호"))
            {
                m_itemPrices[item] = price / (double)10;
            }
            else
            {
                m_itemPrices[item] = price;
            }

            for (RaidReward* pRaidRewardWidget : m_raidRewardWidgets)
            {
                if (item.contains("명예의 파편"))
                    pRaidRewardWidget->updateItemPrice("명예의 파편");
                else
                    pRaidRewardWidget->updateItemPrice(item);
            }
        });
        connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);
        ApiManager::getInstance()->post(pNetworkManager, LostarkApi::Market, QJsonDocument(searchOption).toJson());
    }
}

QJsonObject RaidRewardProfit::buildSearchOption(QString itemName)
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
