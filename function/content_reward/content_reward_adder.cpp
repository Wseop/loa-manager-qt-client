#include "content_reward_adder.h"
#include "ui_content_reward_adder.h"
#include "ui/widget_manager.h"
#include "api/api_manager.h"
#include "api/requestbody_builder.h"
#include "api/loamanager/loamanager_api.h"

#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>

ContentRewardAdder::ContentRewardAdder(const QStringList &contents, const QHash<QString, QStringList> &contentLevels, const QHash<QString, QStringList> &dropTable) :
    ui(new Ui::ContentRewardAdder),
    mContents(contents),
    mContentLevels(contentLevels),
    mDropTable(dropTable),
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
                levelItems << levels[i];
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
    }

    mpCurrentLevelSelector = mLevelSelectors[0];
    mpCurrentLevelSelector->show();
}

void ContentRewardAdder::initializeInsertButton()
{
    QPushButton *pInsertButton = WidgetManager::createPushButton("데이터 추가", 10, 100, 27);
    ui->hLayoutSelector->addWidget(pInsertButton);

    connect(pInsertButton, &QPushButton::released, this, &ContentRewardAdder::insertData);
}

void ContentRewardAdder::initializeDataInputTable()
{
    const QStringList items = {"데이터 누적량", "실링", "명예의 파편", "파괴석", "수호석", "돌파석", "보석"};

    for (int i = 0; i < items.size(); i++)
    {
        QLabel *pLabelItem = WidgetManager::createLabel(items[i], 12);
        ui->gridInputTable->addWidget(pLabelItem, 0, i);

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
}

void ContentRewardAdder::insertData()
{
    const QString &content = mpContentSelector->currentText();
    const QString &level = mpCurrentLevelSelector->currentText();

    int count = mLineEdits[0]->text().toInt();
    QStringList items = mDropTable[level];
    QList<int> itemCounts;

    for (const QString &item : items)
    {
        const QString& text = mLineEdits[mItemIndex[item]]->text();

        if (text == "")
            return;

        itemCounts.append(text.toInt());
    }

    Reward reward = {level, count, items, itemCounts};

    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();

    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    int urlIndex = content == "카오스 던전" ? static_cast<int>(LoamanagerApi::PostRewardChaos) :
                                            static_cast<int>(LoamanagerApi::PostRewardGuardian);

    ApiManager::getInstance()->post(pNetworkManager, ApiType::LoaManager, urlIndex, RequestBodyBuilder::buildRewardBody(reward));

    for (QLineEdit *pLineEdit : mLineEdits)
        pLineEdit->clear();

    this->close();
}
