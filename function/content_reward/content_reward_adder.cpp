#include "content_reward_adder.h"
#include "ui_content_reward_adder.h"
#include "ui/widget_manager.h"
#include "db/document_builder.h"
#include "db/db_manager.h"

#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>
#include <QThread>

ContentRewardAdder::ContentRewardAdder(const QStringList &contents, const QHash<QString, QStringList> &contentLevels, const QHash<QString, QStringList> &dropTable) :
    ui(new Ui::ContentRewardAdder),
    m_contents(contents),
    m_contentLevels(contentLevels),
    m_dropTable(dropTable),
    m_stages({2, 2, 2, 1}),
    m_pContentSelector(nullptr),
    m_pCurrentLevelSelector(nullptr),
    m_pInputValidator(new QIntValidator())
{
    ui->setupUi(this);

    initializeContentSelector();
    initializeLevelSelector();
    initializeInsertButton();
    initializeDataInputTable();

    this->setWindowIcon(QIcon(":/Home.ico"));
    this->setWindowTitle("데이터 추가");
}

ContentRewardAdder::~ContentRewardAdder()
{
    if (m_pContentSelector != nullptr)
        delete m_pContentSelector;
    m_pContentSelector = nullptr;

    for (QComboBox *pLevelSelector : m_levelSelectors)
        delete pLevelSelector;
    m_levelSelectors.clear();

    if (m_pInputValidator != nullptr)
        delete m_pInputValidator;
    m_pInputValidator = nullptr;

    for (QLineEdit *pLineEdit : m_lineEdits)
        delete pLineEdit;
    m_lineEdits.clear();

    for (QWidget *pWidget : m_widgets)
        delete pWidget;
    m_widgets.clear();

    for (QLayout *pLayout : m_layouts)
        delete pLayout;
    m_layouts.clear();

    delete ui;
}

void ContentRewardAdder::initializeContentSelector()
{
    m_pContentSelector = WidgetManager::createComboBox(m_contents);
    ui->hLayoutSelector->addWidget(m_pContentSelector);
    connect(m_pContentSelector, &QComboBox::currentIndexChanged, this, [&](int index){
        m_pCurrentLevelSelector->hide();
        m_pCurrentLevelSelector = m_levelSelectors[index];
        m_pCurrentLevelSelector->show();
    });
}

void ContentRewardAdder::initializeLevelSelector()
{
    for (const QString &content : m_contents)
    {
        QStringList levelItems;

        if (content == "카오스 던전")
        {
            const QStringList &levels = m_contentLevels[content];

            for (int i = 0; i < levels.size(); i++)
            {
                for (int j = 0; j < m_stages[i]; j++)
                {
                    levelItems << levels[i] + QString::number(j + 1);
                }
            }
        }
        else
        {
            levelItems = m_contentLevels[content];
        }

        QComboBox *pLevelSelector = WidgetManager::createComboBox(levelItems);
        pLevelSelector->hide();
        ui->hLayoutSelector->addWidget(pLevelSelector);
        m_levelSelectors.append(pLevelSelector);
        connect(pLevelSelector, &QComboBox::currentIndexChanged, this, [&](int index){

        });
    }

    m_pCurrentLevelSelector = m_levelSelectors[0];
    m_pCurrentLevelSelector->show();
}

void ContentRewardAdder::initializeInsertButton()
{
    QPushButton *pInsertButton = WidgetManager::createPushButton("데이터 추가", 10, 100, 27);
    ui->hLayoutSelector->addWidget(pInsertButton);
    m_widgets.append(pInsertButton);

    connect(pInsertButton, &QPushButton::released, this, &ContentRewardAdder::insertData);
}

void ContentRewardAdder::initializeDataInputTable()
{
    const QStringList items = {"데이터 누적량", "실링", "명예의 파편", "파괴석", "수호석", "돌파석", "보석", "전설 악세", "유물 악세", "고대 악세"};

    for (int i = 0; i < items.size(); i++)
    {
        QLabel *pLabelItem = WidgetManager::createLabel(items[i], 12);
        ui->gridInputTable->addWidget(pLabelItem, 0, i);
        m_widgets.append(pLabelItem);

        QLineEdit *pLineEdit = WidgetManager::createLineEdit(m_pInputValidator, "", 10, 100, 25);
        ui->gridInputTable->addWidget(pLineEdit, 1, i);
        m_lineEdits.append(pLineEdit);
    }

    m_itemIndex["실링"] = 1;
    m_itemIndex["명예의 파편"] = 2;
    m_itemIndex["파괴강석"] = 3;
    m_itemIndex["정제된 파괴강석"] = 3;
    m_itemIndex["수호강석"] = 4;
    m_itemIndex["정제된 수호강석"] = 4;
    m_itemIndex["경이로운 명예의 돌파석"] = 5;
    m_itemIndex["찬란한 명예의 돌파석"] = 5;
    m_itemIndex["보석"] = 6;
    m_itemIndex["전설 악세"] = 7;
    m_itemIndex["유물 악세"] = 8;
    m_itemIndex["고대 악세"] = 9;
}

void ContentRewardAdder::insertData()
{
    const QString &content = m_pContentSelector->currentText();
    int count = m_lineEdits[0]->text().toInt();
    const QString &level = m_pCurrentLevelSelector->currentText();

    QStringList items;
    if (content == "카오스 던전")
        items = m_dropTable[level.chopped(1)];
    else
        items = m_dropTable[level];

    QList<int> itemCounts;
    for (const QString &item : items)
    {
        const QString& text = m_lineEdits[m_itemIndex[item]]->text();
        if (text == "")
            return;

        itemCounts.append(text.toInt());
    }

    Collection collection = content == "카오스 던전" ? Collection::Reward_Chaos : Collection::Reward_Guardian;

    QThread *pInsertThread = QThread::create(tInsertData, collection, count, level, items, itemCounts);
    connect(pInsertThread, &QThread::finished, pInsertThread, &QThread::deleteLater);
    pInsertThread->start();

    for (QLineEdit *pLineEdit : m_lineEdits)
        pLineEdit->clear();

    this->close();
}

void ContentRewardAdder::tInsertData(Collection collection, int count, QString level, QStringList items, QList<int> itemCounts)
{
    DbManager *pDbManager = DbManager::getInstance();

    bsoncxx::builder::stream::document doc = DocumentBuilder::buildDocumentContentsReward(count, level, items, itemCounts);
    bsoncxx::builder::stream::document dummyFilter = {};

    pDbManager->lock();
    pDbManager->insertDocument(collection, doc.extract(), dummyFilter.extract(), true);
    pDbManager->unlock();
}
