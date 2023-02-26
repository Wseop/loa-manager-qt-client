#include "reward_adder.h"
#include "ui_reward_adder.h"
#include "ui/widget_manager.h"
#include "db/db_manager.h"
#include "db/document_builder.h"
#include "game_data/character/item/enum/item_grade.h"
#include "resource/resource_manager.h"

#include <QGridLayout>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QIntValidator>
#include <QMessageBox>
#include <QThread>

RewardAdder::RewardAdder(const QString& content, const QHash<QString, QStringList>& dropTable) :
    ui(new Ui::RewardAdder),
    m_content(content),
    m_dropTable(dropTable),
    m_pInputValidator(new QIntValidator()),
    m_pLevelSelector(nullptr),
    m_pGridLayout(nullptr)
{
    ui->setupUi(this);
    ui->vLayoutMain->setAlignment(Qt::AlignTop);
    this->setWindowIcon(QIcon(":/icon/Home.ico"));
    this->setWindowTitle(content + " 데이터 추가");

    initIconPath();
    initLevels();
    initLevelSelector();
    initInputList(m_levels[0]);
}

RewardAdder::~RewardAdder()
{
    for (QWidget* pWidget : m_widgets)
        delete pWidget;
    for (QWidget* pWidget : m_inputWidgets)
        delete pWidget;
    delete m_pInputValidator;
    delete m_pGridLayout;
    delete ui;
}

void RewardAdder::initIconPath()
{
    const QJsonObject json = ResourceManager::getInstance()->loadJson("reforge");

    QString iconPath = ":/reforge/image/reforge/reforge_%1_%2.png";
    const QJsonArray& reforges = json.find("Reforge")->toArray();
    for (int i = 0; i < reforges.size(); i++)
    {
        const QJsonArray& items = reforges[i].toObject().find("Items")->toArray();
        for (int j = 0; j < items.size(); j++)
        {
            const QJsonObject& item = items[j].toObject();
            m_iconPaths[item.find("Name")->toString()] = iconPath.arg(i).arg(j);
        }
    }

    m_iconPaths["실링"] = ":/money/image/money/money_0.png";
    m_iconPaths["보석"] = ":/item/image/item/gem_0.png";
}

void RewardAdder::initLevels()
{
    if (m_content == "카오스 던전")
        m_levels = {"타락1", "타락2", "타락3", "공허1", "공허2", "절망1", "절망2", "천공1", "천공2", "계몽1"};
    else if (m_content == "가디언 토벌")
        m_levels = {"칼엘리고스", "하누마탄", "소나벨", "가르가디스"};
}

void RewardAdder::initLevelSelector()
{
    // label 추가
    QLabel* pLabel = WidgetManager::createLabel("단계 선택");
    ui->hLayoutSelector->addWidget(pLabel);
    m_widgets.append(pLabel);

    // level selector(combobox) 추가
    m_pLevelSelector = WidgetManager::createComboBox(m_levels);
    ui->hLayoutSelector->addWidget(m_pLevelSelector);
    connect(m_pLevelSelector, &QComboBox::currentIndexChanged, this, [&](int index){
        initInputList(m_levels[index]);
    });
    m_widgets.append(m_pLevelSelector);

    ui->hLayoutSelector->setAlignment(Qt::AlignHCenter);
}

void RewardAdder::initInputList(QString level)
{
    if (m_content == "카오스 던전")
        level.chop(1);

    // ui 초기화 및 추가
    if (m_pGridLayout != nullptr)
    {
        for (QWidget* pWidget : m_inputWidgets)
            delete pWidget;
        m_inputWidgets.clear();
        delete m_pGridLayout;
    }
    m_lineEdits.clear();
    m_pGridLayout = new QGridLayout();
    ui->vLayoutMain->addLayout(m_pGridLayout);

    const QStringList& items = m_dropTable[level];
    // 아이템 목록 추가
    int col = 0;
    for (const QString& item : items)
    {
        QLabel* pLabelItem = nullptr;

        if (item == "전설" || item == "유물" || item == "고대")
            pLabelItem = WidgetManager::createLabel(item + "\n악세", 10, colorCode(strToItemGrade(item)), 50, 50);
        else
            pLabelItem = WidgetManager::createIcon(m_iconPaths[item], nullptr);

        m_pGridLayout->addWidget(pLabelItem, 0, col++);
        m_pGridLayout->setAlignment(pLabelItem, Qt::AlignHCenter);
        m_inputWidgets.append(pLabelItem);
    }
    QLabel* pLabelRemark = WidgetManager::createLabel("비고");
    m_pGridLayout->addWidget(pLabelRemark, 0, col++);
    m_inputWidgets.append(pLabelRemark);

    // 아이템 수량 입력칸 추가
    col = 0;
    for (int i = 0; i < items.size(); i++)
    {
        QLineEdit* pLineEdit = WidgetManager::createLineEdit(m_pInputValidator, "수량 입력");
        m_pGridLayout->addWidget(pLineEdit, 1, col++);
        m_inputWidgets.append(pLineEdit);
        m_lineEdits.append(pLineEdit);
    }
    QLineEdit* pLineEdit = WidgetManager::createLineEdit(nullptr);
    m_pGridLayout->addWidget(pLineEdit, 1, col++);
    m_inputWidgets.append(pLineEdit);
    m_lineEdits.append(pLineEdit);

    QPushButton* pAddButton = WidgetManager::createPushButton("추가", 10, 100 * (items.size() + 1) + (9 * items.size()), 50);
    m_pGridLayout->addWidget(pAddButton, 2, 0, 1, -1);
    connect(pAddButton, &QPushButton::released, this, &RewardAdder::addData);
    m_pGridLayout->setAlignment(pAddButton, Qt::AlignHCenter);
    m_inputWidgets.append(pAddButton);
}

void RewardAdder::addData()
{
    bool bDataReady = true;

    for (int i = 0; i < m_lineEdits.size() - 1; i++)
    {
        if (m_lineEdits[i]->text() == "")
        {
            QMessageBox msgBox;
            msgBox.setText("수량을 모두 채워주세요.");
            msgBox.exec();
            bDataReady = false;
            break;
        }
    }

    if (bDataReady)
    {
        // db에 추가할 데이터 초기화
        QString level = m_levels[m_pLevelSelector->currentIndex()];

        QStringList items;
        if (m_content == "카오스 던전")
            items = m_dropTable[level.chopped(1)];
        else
            items = m_dropTable[level];

        QList<int> itemCounts;
        for (int i = 0; i < m_lineEdits.size() - 1; i++)
            itemCounts.append(m_lineEdits[i]->text().toInt());

        QThread* pThread = QThread::create(tInsertData, m_content, level, items, itemCounts, m_lineEdits.back()->text());
        connect(pThread, &QThread::finished, pThread, &QThread::deleteLater);
        pThread->start();

        for (int i = 0; i < m_lineEdits.size(); i++)
            m_lineEdits[i]->clear();
        this->close();
    }
}

void RewardAdder::tInsertData(QString content, QString level, QStringList items, QList<int> itemCounts, QString remark)
{
    DbManager* pDbManager = DbManager::getInstance();
    bsoncxx::builder::stream::document dummyFilter{};

    pDbManager->lock();

    if (content == "카오스 던전")
        pDbManager->insertDocument(Collection::Reward_Chaos, DocumentBuilder::buildDocumentContentsReward(level, items, itemCounts, remark).extract(), dummyFilter.extract(), true);
    else if (content == "가디언 토벌")
        pDbManager->insertDocument(Collection::Reward_Guardian, DocumentBuilder::buildDocumentContentsReward(level, items, itemCounts, remark).extract(), dummyFilter.extract(), true);

    pDbManager->unlock();
}
