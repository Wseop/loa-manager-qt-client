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
    mContents(contents),
    mContentLevels(contentLevels),
    mDropTable(dropTable),
    mStages({2, 2, 2, 1}),
    mpContentSelector(nullptr),
    mpCurrentLevelSelector(nullptr),
    mpInputValidator(new QIntValidator())
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
    if (mpContentSelector != nullptr)
        delete mpContentSelector;
    mpContentSelector = nullptr;

    for (QComboBox *pLevelSelector : mLevelSelectors)
        delete pLevelSelector;
    mLevelSelectors.clear();

    if (mpInputValidator != nullptr)
        delete mpInputValidator;
    mpInputValidator = nullptr;

    for (QLineEdit *pLineEdit : mLineEdits)
        delete pLineEdit;
    mLineEdits.clear();

    for (QWidget *pWidget : mWidgets)
        delete pWidget;
    mWidgets.clear();

    for (QLayout *pLayout : mLayouts)
        delete pLayout;
    mLayouts.clear();

    delete ui;
}

void ContentRewardAdder::initializeContentSelector()
{
    mpContentSelector = WidgetManager::createComboBox(mContents);
    ui->hLayoutSelector->addWidget(mpContentSelector);
    connect(mpContentSelector, &QComboBox::currentIndexChanged, this, [&](int index){
        mpCurrentLevelSelector->hide();
        mpCurrentLevelSelector = mLevelSelectors[index];
        mpCurrentLevelSelector->show();
    });
}

void ContentRewardAdder::initializeLevelSelector()
{
    for (const QString &content : mContents)
    {
        QStringList levelItems;

        if (content == "카오스 던전")
        {
            const QStringList &levels = mContentLevels[content];

            for (int i = 0; i < levels.size(); i++)
            {
                for (int j = 0; j < mStages[i]; j++)
                {
                    levelItems << levels[i] + QString::number(j + 1);
                }
            }
        }
        else
        {
            levelItems = mContentLevels[content];
        }

        QComboBox *pLevelSelector = WidgetManager::createComboBox(levelItems);
        pLevelSelector->hide();
        ui->hLayoutSelector->addWidget(pLevelSelector);
        mLevelSelectors.append(pLevelSelector);
        connect(pLevelSelector, &QComboBox::currentIndexChanged, this, [&](int index){

        });
    }

    mpCurrentLevelSelector = mLevelSelectors[0];
    mpCurrentLevelSelector->show();
}

void ContentRewardAdder::initializeInsertButton()
{
    QPushButton *pInsertButton = WidgetManager::createPushButton("데이터 추가", 10, 100, 27);
    ui->hLayoutSelector->addWidget(pInsertButton);
    mWidgets.append(pInsertButton);

    connect(pInsertButton, &QPushButton::released, this, &ContentRewardAdder::insertData);
}

void ContentRewardAdder::initializeDataInputTable()
{
    const QStringList items = {"데이터 누적량", "실링", "명예의 파편", "파괴석", "수호석", "돌파석", "보석", "전설 악세", "유물 악세", "고대 악세"};

    for (int i = 0; i < items.size(); i++)
    {
        QLabel *pLabelItem = WidgetManager::createLabel(items[i], 12);
        ui->gridInputTable->addWidget(pLabelItem, 0, i);
        mWidgets.append(pLabelItem);

        QLineEdit *pLineEdit = WidgetManager::createLineEdit(mpInputValidator, "", 10, 100, 25);
        ui->gridInputTable->addWidget(pLineEdit, 1, i);
        mLineEdits.append(pLineEdit);
    }

    mItemIndex["실링"] = 1;
    mItemIndex["명예의 파편"] = 2;
    mItemIndex["파괴강석"] = 3;
    mItemIndex["정제된 파괴강석"] = 3;
    mItemIndex["수호강석"] = 4;
    mItemIndex["정제된 수호강석"] = 4;
    mItemIndex["경이로운 명예의 돌파석"] = 5;
    mItemIndex["찬란한 명예의 돌파석"] = 5;
    mItemIndex["보석"] = 6;
    mItemIndex["전설 악세"] = 7;
    mItemIndex["유물 악세"] = 8;
    mItemIndex["고대 악세"] = 9;
}

void ContentRewardAdder::insertData()
{
    const QString &content = mpContentSelector->currentText();
    int count = mLineEdits[0]->text().toInt();
    const QString &level = mpCurrentLevelSelector->currentText();

    QStringList items;
    if (content == "카오스 던전")
        items = mDropTable[level.chopped(1)];
    else
        items = mDropTable[level];

    QList<int> itemCounts;
    for (const QString &item : items)
    {
        const QString& text = mLineEdits[mItemIndex[item]]->text();
        if (text == "")
            return;

        itemCounts.append(text.toInt());
    }

    Collection collection = content == "카오스 던전" ? Collection::Reward_Chaos : Collection::Reward_Guardian;

    QThread *pInsertThread = QThread::create(tInsertData, collection, count, level, items, itemCounts);
    connect(pInsertThread, &QThread::finished, pInsertThread, &QThread::deleteLater);
    pInsertThread->start();

    for (QLineEdit *pLineEdit : mLineEdits)
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
