#include "contents_reward.h"
#include "ui_contents_reward.h"
#include "ui/widget_manager.h"
#include "function/contents_reward/reward_widget.h"
#include "function/contents_reward/reward_adder.h"
#include "db/db_manager.h"
#include "api/api_manager.h"
#include "api/search_option.h"
#include "resource/resource_manager.h"

#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QFile>
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
    m_pContentsSelector(nullptr)
{
    ui->setupUi(this);
    ui->hLayoutInput->setAlignment(Qt::AlignHCenter);
    ui->vLayoutOutput->setAlignment(Qt::AlignHCenter);

    loadDropTable();

    initRefreshButton();
    initContentsSelector();
    initInsertDataButton();
    initRewardAdder();
    initInfoLabel();

    initChaosReward();
    initGuardianReward();
}

ContentsReward::~ContentsReward()
{
    for (QWidget* pWidget : m_widgets)
        delete pWidget;
    delete ui;
}

void ContentsReward::loadDropTable()
{
    const QStringList files = {"drop_chaos", "drop_guardian"};

    // load drop tables
    for (const QString& file : files)
    {
        const QJsonObject json = ResourceManager::getInstance()->loadJson(file);

        // add content name
        m_contents << json.find("Content")->toString();

        // add drop table
        QHash<QString, QStringList> dropTable;
        const QJsonArray& jsonDropTable = json.find("DropTable")->toArray();
        for (const QJsonValue& value : jsonDropTable)
        {
            const QJsonObject& obj = value.toObject();
            const QString& level = obj.find("Level")->toString();
            dropTable[level] = obj.find("ItemList")->toVariant().toStringList();
        }
        m_dropTables.append(dropTable);
    }

    m_rewardWidgets.resize(m_contents.size());
}

void ContentsReward::initRefreshButton()
{
    QPushButton* pButtonRefresh = WidgetManager::createPushButton(QPixmap(":/icon/image/refresh.png"));
    ui->hLayoutInput->addWidget(pButtonRefresh);

    // 클릭 시 가격정보 새로고침
    connect(pButtonRefresh, &QPushButton::released, this, &ContentsReward::refreshPrice);

    m_widgets.append(pButtonRefresh);
}

void ContentsReward::initContentsSelector()
{
    m_pContentsSelector = WidgetManager::createComboBox(m_contents);
    ui->hLayoutInput->addWidget(m_pContentsSelector);

    // 선택한 컨텐츠로 출력 내용 변경
    connect(m_pContentsSelector, &QComboBox::currentIndexChanged, this, [&](int index){
        for (int i = 0; i < m_rewardWidgets.size(); i++)
        {
            const QList<RewardWidget*>& rewardWidgets = m_rewardWidgets[i];

            if (i == index)
                for (RewardWidget* pRewardWidget : rewardWidgets)
                    pRewardWidget->show();
            else
                for (RewardWidget* pRewardWidget : rewardWidgets)
                    pRewardWidget->hide();
        }
    });

    m_widgets.append(m_pContentsSelector);
}

void ContentsReward::initInsertDataButton()
{
    QPushButton* pButtonInsertData = WidgetManager::createPushButton("데이터 추가");
    ui->hLayoutInput->addWidget(pButtonInsertData);

    // 선택된 컨텐츠에 해당하는 widget(RewardAdder) popup
    connect(pButtonInsertData, &QPushButton::released, this, [&](){
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

    m_widgets.append(pButtonInsertData);
}

void ContentsReward::initRewardAdder()
{
    for (int i = 0; i < m_contents.size(); i++)
    {
        RewardAdder* pRewardAdder = new RewardAdder(m_contents[i], m_dropTables[i]);
        m_rewardAdders.append(pRewardAdder);
        m_widgets.append(pRewardAdder);
    }
}

void ContentsReward::initInfoLabel()
{
    const QStringList infoTexts = {"※ 2수 기준 평균 획득량 (휴식 게이지X)",
                                   "※ 골드는 명예의 파편, 파괴석, 수호석, 돌파석, 보석을 골드로 환산한 금액 (거래소 기준)"};

    for (int i = 0; i < MAX_INFO; i++)
    {
        QLabel* pLabelInfo = WidgetManager::createLabel(infoTexts[i], 12, "", 1000);
        ui->vLayoutOutput->addWidget(pLabelInfo);
        ui->vLayoutOutput->setAlignment(pLabelInfo, Qt::AlignHCenter);
        m_widgets.append(pLabelInfo);
    }
}

void ContentsReward::initChaosReward()
{
    const int contentIndex = 0;
    const QHash<QString, QStringList>& dropTable = m_dropTables[contentIndex];
    const QStringList& levels = {"타락", "공허", "절망", "천공", "계몽"};
    const QList<int> levelCounts = {3, 2, 2, 2, 1};

    // db에서 카오스 던전 reward data 로드
    DbManager* pDbManager = DbManager::getInstance();
    bsoncxx::builder::stream::document filter{};
    filter << "Remark" << "";

    pDbManager->lock();
    const QJsonArray data = pDbManager->findDocuments(Collection::Reward_Chaos, SortOrder::None, "", filter.extract());
    pDbManager->unlock();

    // reward data를 level별로 취합
    QHash<QString, QList<QJsonObject>> rewardDatas;
    for (const QJsonValue& value : data)
    {
        const QJsonObject& rewardData = value.toObject();

        for (const QString& level : levels)
        {
            if (rewardData.find("Level")->toString().contains(level))
            {
                rewardDatas[level].append(rewardData);
                break;
            }
        }
    }

    // reward data를 RewardWidget형식으로 화면에 출력
    for (int i = 0; i < levels.size(); i++)
    {
        RewardWidget* pChaosReward = new RewardWidget(ContentType::Chaos, levels[i], levelCounts[i], dropTable[levels[i]], rewardDatas[levels[i]]);
        ui->vLayoutOutput->addWidget(pChaosReward);
        m_rewardWidgets[contentIndex].append(pChaosReward);
        m_widgets.append(pChaosReward);
    }
}

void ContentsReward::initGuardianReward()
{
    const int contentIndex = 1;
    const QHash<QString, QStringList>& dropTable = m_dropTables[contentIndex];
    const QStringList& levels = {"칼엘리고스", "하누마탄", "소나벨", "가르가디스"};

    // db에서 가디언 토벌 reward data 로드
    DbManager* pDbManager = DbManager::getInstance();
    bsoncxx::builder::stream::document filter{};
    filter << "Remark" << "";

    pDbManager->lock();
    QJsonArray data = pDbManager->findDocuments(Collection::Reward_Guardian, SortOrder::None, "", filter.extract());
    pDbManager->unlock();

    // reward data를 level별로 취합
    QHash<QString, QList<QJsonObject>> rewardDatas;
    for (const QJsonValue& value : data)
    {
        const QJsonObject& rewardData = value.toObject();
        rewardDatas[rewardData.find("Level")->toString()].append(rewardData);
    }

    // reward data를 RewardWidget형식으로 화면에 출력
    for (const QString& level : levels)
    {
        RewardWidget* pGuardianWidget = new RewardWidget(ContentType::Guardian, level, 1, dropTable[level], rewardDatas[level]);
        ui->vLayoutOutput->addWidget(pGuardianWidget);
        m_rewardWidgets[contentIndex].append(pGuardianWidget);
        m_widgets.append(pGuardianWidget);

        pGuardianWidget->hide();
    }
}

void ContentsReward::refreshPrice()
{
    QStringList items = {"명예의 파편 주머니(소)",
                         "파괴석 결정", "파괴강석", "정제된 파괴강석",
                         "수호석 결정", "수호강석", "정제된 수호강석",
                         "위대한 명예의 돌파석", "경이로운 명예의 돌파석", "찬란한 명예의 돌파석",
                         "보석"};
    SearchOption searchOption(SearchType::Market);
    searchOption.setItemTier(3);
    searchOption.setPageNo(1);
    searchOption.setSortCondition("ASC");

    for (const QString& item : items)
    {
        if (item == "보석")
        {
            searchOption.setSearchType(SearchType::Auction);
            searchOption.setCategoryCode(CategoryCode::Gem);
            searchOption.setItemName("1레벨 멸화");
        }
        else
        {
            searchOption.setSearchType(SearchType::Market);
            searchOption.setCategoryCode(CategoryCode::Reforge);
            searchOption.setItemName(item);
        }

        QNetworkAccessManager* pNetworkManager = new QNetworkAccessManager();
        connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&, item](QNetworkReply* pReply){
            QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
            if (response.isNull())
                return;

            const QJsonArray& jsonItems = response.object().find("Items")->toArray();
            if (jsonItems.size() == 0)
                return;

            // parse price
            int price = 0;
            if (item == "보석")
                price = jsonItems[0].toObject().find("AuctionInfo")->toObject().find("BuyPrice")->toInt();
            else
                price = jsonItems[0].toObject().find("CurrentMinPrice")->toInt();

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

            // RewardWidget의 price 업데이트
            for (int i = 0; i < m_contents.size(); i++)
            {
                for (RewardWidget* pRewardWidget : m_rewardWidgets[i])
                {
                    if (item.contains("명예의 파편"))
                        pRewardWidget->updatePrice("명예의 파편", m_itemPrices["명예의 파편"]);
                    else
                        pRewardWidget->updatePrice(item, m_itemPrices[item]);
                }
            }
        });

        connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);
        if (item == "보석")
            ApiManager::getInstance()->post(pNetworkManager, LostarkApi::Auction, QJsonDocument(searchOption.toJsonObject()).toJson());
        else
            ApiManager::getInstance()->post(pNetworkManager, LostarkApi::Market, QJsonDocument(searchOption.toJsonObject()).toJson());
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
