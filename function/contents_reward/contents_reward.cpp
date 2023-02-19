#include "contents_reward.h"
#include "ui_contents_reward.h"
#include "ui/widget_manager.h"
#include "function/contents_reward/reward_widget.h"
#include "function/contents_reward/reward_adder.h"
#include "db/db_manager.h"
#include "api/api_manager.h"
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QFile>
#include <QSettings>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>

extern bool g_bAdmin;

ContentsReward* ContentsReward::m_pInstance = nullptr;

ContentsReward::ContentsReward() :
    ui(new Ui::ContentsReward),
    m_rewardWidgets(MAX_CONTENTS)
{
    ui->setupUi(this);

    initContents();
    loadDropTable();

    setAlignments();
    initRefreshButton();
    initContentsSelector();
    initInputDataButton();
    initInfoLabel();

    initRewardAdder();
    initChaosReward();
}

ContentsReward::~ContentsReward()
{
    for (QWidget* pWidget : m_widgets)
        delete pWidget;
    delete ui;
}

void ContentsReward::initContents()
{
    m_contents = {"카오스 던전", "가디언 토벌"};
}

void ContentsReward::loadDropTable()
{
    // load 카오스 던전 drop table
    {
        QFile file(":/json/json/drop_table/drop_chaos.json");
        if (!file.open(QIODevice::ReadOnly))
        {
            qDebug() << Q_FUNC_INFO << "drop_chaos.json load fail";
            return;
        }

        QJsonObject dropChaos = QJsonDocument::fromJson(file.readAll()).object();
        const QJsonArray& dropTables = dropChaos.find("DropTable")->toArray();
        QHash<QString, QStringList> dropTable;
        for (int i = 0; i < dropTables.size(); i++)
        {
            const QJsonObject& objDropTable = dropTables[i].toObject();
            const QString& level = objDropTable.find("Level")->toString();
            const QJsonArray& jArrItemList = objDropTable.find("ItemList")->toArray();

            QStringList itemList;
            for (int j = 0; j < jArrItemList.size(); j++)
                itemList << jArrItemList[j].toString();

            dropTable[level] = itemList;
        }

        m_dropTables.append(dropTable);
        file.close();
    }

    // load 가디언 토벌 drop table
    {
        // dummy
        QHash<QString, QStringList> dropTable;
        m_dropTables.append(dropTable);
    }
}

void ContentsReward::setAlignments()
{
    ui->hLayoutInput->setAlignment(Qt::AlignHCenter);
    ui->vLayoutOutput->setAlignment(Qt::AlignHCenter);
}

void ContentsReward::initRefreshButton()
{
    QPushButton* pButtonRefresh = WidgetManager::createPushButton(QPixmap(":/icon/image/refresh.png"));
    ui->hLayoutInput->addWidget(pButtonRefresh);
    connect(pButtonRefresh, &QPushButton::released, this, &ContentsReward::refreshPrice);

    m_widgets.append(pButtonRefresh);
}

void ContentsReward::initContentsSelector()
{
    m_pContentsSelector = WidgetManager::createComboBox(m_contents);
    ui->hLayoutInput->addWidget(m_pContentsSelector);
    connect(m_pContentsSelector, &QComboBox::currentIndexChanged, this, [&](int index){
        for (int i = 0; i < m_rewardWidgets.size(); i++)
        {
            QList<RewardWidget*>& rewardWidgets = m_rewardWidgets[i];

            if (i == index)
            {
                for (RewardWidget* pRewardWidget : rewardWidgets)
                    pRewardWidget->show();
            }
            else
            {
                for (RewardWidget* pRewardWidget : rewardWidgets)
                    pRewardWidget->hide();
            }
        }
    });

    m_widgets.append(m_pContentsSelector);
}

void ContentsReward::initInputDataButton()
{
    QPushButton* pButtonInputData = WidgetManager::createPushButton("데이터 추가");
    ui->hLayoutInput->addWidget(pButtonInputData);
    connect(pButtonInputData, &QPushButton::released, this, [&](){
        int currentIndex = m_pContentsSelector->currentIndex();

        if (!g_bAdmin)
        {
            QMessageBox msgBox;
            msgBox.setText("관리자 권한 필요");
            msgBox.exec();
        }
        else
        {
            m_rewardAdders[currentIndex]->show();
        }
    });
}

void ContentsReward::initInfoLabel()
{
    for (int i = 0; i < MAX_INFO; i++)
    {
        QLabel* pLabelInfo = WidgetManager::createLabel("", 12, "", 1000);
        ui->vLayoutOutput->addWidget(pLabelInfo);
        ui->vLayoutOutput->setAlignment(pLabelInfo, Qt::AlignHCenter);
        m_infoLabels.append(pLabelInfo);
        m_widgets.append(pLabelInfo);
    }
}

void ContentsReward::initRewardAdder()
{
    for (int i = 0; i < MAX_CONTENTS; i++)
    {
        RewardAdder* pRewardAdder = new RewardAdder(m_contents[i], m_dropTables[i]);
        m_rewardAdders.append(pRewardAdder);
        m_widgets.append(pRewardAdder);
    }
}

void ContentsReward::initChaosReward()
{
    const QHash<QString, QStringList>& dropTable = m_dropTables[0];
    const QStringList& levels = {"타락", "공허", "절망", "천공"};
    const QList<int> levelCounts = {3, 2, 2, 2};

    // info label
    QStringList infoTexts = {"※ 2수 기준 평균 획득량 (휴식X)",
                             "※ 골드는 명예의 파편, 파괴석, 수호석, 돌파석, 보석을 골드로 환산한 금액입니다. (거래소 기준)"};
    for (int i = 0; i < MAX_INFO; i++)
    {
        m_infoLabels[i]->setText(infoTexts[i]);
    }

    // load data
    QHash<QString, QList<QJsonObject>> rewardData;
    DbManager* pDbManager = DbManager::getInstance();
    bsoncxx::builder::stream::document filter{};
    filter << "Remark" << "";

    pDbManager->lock();
    QJsonArray data = pDbManager->findDocuments(Collection::Reward_Chaos, SortOrder::None, "", filter.extract());
    pDbManager->unlock();

    for (int i = 0 ;i < data.size(); i++)
    {
        const QJsonObject& reward = data[i].toObject();
        const QString& level = reward.find("Level")->toString();
        for (const QString& key : levels)
        {
            if (level.contains(key))
            {
                rewardData[key].append(reward);
                break;
            }
        }
    }

    // add widgets
    for (int i = 0; i < levels.size(); i++)
    {
        RewardWidget* pChaosReward = new RewardWidget(ContentType::Chaos, levels[i], levelCounts[i], dropTable[levels[i]], rewardData[levels[i]]);
        ui->vLayoutOutput->addWidget(pChaosReward);
        m_rewardWidgets[0].append(pChaosReward);
        m_widgets.append(pChaosReward);
    }
}

QJsonObject ContentsReward::buildSearchOption(QString itemName)
{
    QJsonObject searchOption;

    searchOption.insert("ItemTier", 3);
    searchOption.insert("PageNo", 1);
    searchOption.insert("SortCondition", "ASC");

    if (itemName == "보석")
    {
        searchOption.insert("ItemName", "1레벨 멸화");
        searchOption.insert("Sort", "BUY_PRICE");
        searchOption.insert("CategoryCode", 210000);

    }
    else
    {
        searchOption.insert("ItemName", itemName);
        searchOption.insert("Sort", "CURRENT_MIN_PRICE");
        searchOption.insert("CategoryCode", 50000);
    }

    return searchOption;
}

void ContentsReward::refreshPrice()
{
    QStringList items = {"명예의 파편 주머니(소)",
                         "파괴석 결정", "파괴강석", "정제된 파괴강석",
                         "수호석 결정", "수호강석", "정제된 수호강석",
                         "위대한 명예의 돌파석", "경이로운 명예의 돌파석", "찬란한 명예의 돌파석",
                         "보석"};

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

            int price = 0;
            if (item == "보석")
                price = jArrItems[0].toObject().find("AuctionInfo")->toObject().find("BuyPrice")->toInt();
            else
                price = jArrItems[0].toObject().find("CurrentMinPrice")->toInt();

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

            // update widget
            for (int i = 0; i < MAX_CONTENTS; i++)
            {
                for (RewardWidget* pRewardWidget : m_rewardWidgets[i])
                    pRewardWidget->updatePrice(item, price);
            }
        });

        connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);
        if (item == "보석")
            ApiManager::getInstance()->post(pNetworkManager, LostarkApi::Auction, QJsonDocument(searchOption).toJson());
        else
            ApiManager::getInstance()->post(pNetworkManager, LostarkApi::Market, QJsonDocument(searchOption).toJson());
    }
}

ContentsReward* ContentsReward::getInstance()
{
    if (m_pInstance == nullptr)
        m_pInstance = new ContentsReward();
    return m_pInstance;
}

void ContentsReward::destroyInstance()
{
    if (m_pInstance == nullptr)
        return;
    delete m_pInstance;
    m_pInstance = nullptr;
}
