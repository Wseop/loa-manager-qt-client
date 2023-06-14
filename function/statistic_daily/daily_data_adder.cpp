#include "daily_data_adder.h"
#include "ui_daily_data_adder.h"
#include "function/statistic_daily/statistic_daily.h"
#include "ui/widget_manager.h"
#include "resource/resource_manager.h"
#include "api/api_manager.h"

#include <QCloseEvent>
#include <QIntValidator>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

DailyDataAdder::DailyDataAdder(const QString &content, const QString &level, const QStringList &itemNames, const QHash<QString, QString> &itemKeyMap) :
    ui(new Ui::DailyDataAdder),
    pIntValidator(new QIntValidator(0, 999999))
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":/Home.ico"));
    this->setWindowTitle("일일 컨텐츠 데이터 추가");

    mRewardChaos.setLevel(level);
    mRewardGuardian.setLevel(level);

    ui->hLayoutMain->addWidget(createLevelGroup(level));
    ui->hLayoutMain->addWidget(createDataCountGroup());
    ui->hLayoutMain->addWidget(createItemGroup(itemNames, itemKeyMap));
    ui->hLayoutMain->addWidget(createAddButton(content));
}

DailyDataAdder::~DailyDataAdder()
{
    delete pIntValidator;
    delete ui;
}

void DailyDataAdder::closeEvent(QCloseEvent *event)
{
    StatisticDaily::getInstance()->setEnabled(true);
    StatisticDaily::getInstance()->refresh();

    event->accept();

    delete this;
}

QGroupBox *DailyDataAdder::createLevelGroup(const QString &level)
{
    QGroupBox *pLevelGroup = WidgetManager::createGroupBox("Level");

    QVBoxLayout *pLevelGroupLayout = new QVBoxLayout();
    pLevelGroupLayout->addWidget(createLevelLabel(level));

    pLevelGroup->setLayout(pLevelGroupLayout);

    return pLevelGroup;
}

QLabel *DailyDataAdder::createLevelLabel(const QString &level)
{
    QLabel *pLevelLabel = WidgetManager::createLabel(level);
    return pLevelLabel;
}

QGroupBox *DailyDataAdder::createDataCountGroup()
{
    QGroupBox *pCountGroup = WidgetManager::createGroupBox("Count");

    QVBoxLayout *pCountGroupLayout = new QVBoxLayout();
    pCountGroupLayout->addWidget(createDataCountField());

    pCountGroup->setLayout(pCountGroupLayout);

    return pCountGroup;
}

QLineEdit *DailyDataAdder::createDataCountField()
{
    QLineEdit *pCountField = WidgetManager::createLineEdit(pIntValidator);

    connect(pCountField, &QLineEdit::textChanged, this, [&](const QString &text)
    {
        mRewardChaos.setCount(text.toInt());
        mRewardGuardian.setCount(text.toInt());
    });

    pCountField->setText("0");

    return pCountField;
}

QGroupBox *DailyDataAdder::createItemGroup(const QStringList &itemNames, const QHash<QString, QString> &itemKeyMap)
{
    QGroupBox *pItemGroup = WidgetManager::createGroupBox("Items");

    QHBoxLayout *pItemGroupLayout = new QHBoxLayout();

    for (const QString &itemName : itemNames)
        pItemGroupLayout->addLayout(createItemLayout(itemName, itemKeyMap[itemName]));

    pItemGroup->setLayout(pItemGroupLayout);

    return pItemGroup;
}

QVBoxLayout *DailyDataAdder::createItemLayout(const QString &itemName, const QString &itemKey)
{
    QVBoxLayout *pItemLayout = new QVBoxLayout();

    pItemLayout->addWidget(createItemIcon(itemName), 0, Qt::AlignHCenter);
    pItemLayout->addWidget(createItemCountField(itemKey), 0, Qt::AlignHCenter);

    return pItemLayout;
}

QLabel *DailyDataAdder::createItemIcon(const QString &itemName)
{
    QLabel *pItemIcon = WidgetManager::createIcon(ResourceManager::getInstance()->iconPath(itemName),
                                                  nullptr);
    return pItemIcon;
}

QLineEdit *DailyDataAdder::createItemCountField(const QString &itemKey)
{
    QLineEdit *pItemCountField = WidgetManager::createLineEdit(pIntValidator);

    connect(pItemCountField, &QLineEdit::textChanged,
            this, [&, itemKey](const QString &text)
    {
        if (itemKey == "silling")
        {
            mRewardChaos.setSilling(text.toInt());
        }
        else if (itemKey == "shard")
        {
            mRewardChaos.setShard(text.toInt());
        }
        else if (itemKey == "destructionStone")
        {
            mRewardChaos.setDestructionStone(text.toInt());
            mRewardGuardian.setDestructionStone(text.toInt());
        }
        else if (itemKey == "protectionStone")
        {
            mRewardChaos.setProtectionStone(text.toInt());
            mRewardGuardian.setProtectionStone(text.toInt());
        }
        else if (itemKey == "leapStone")
        {
            mRewardChaos.setLeapStone(text.toInt());
            mRewardGuardian.setLeapStone(text.toInt());
        }
        else if (itemKey == "gem")
        {
            mRewardChaos.setGem(text.toInt());
        }
    });

    pItemCountField->setText("0");

    return pItemCountField;
}

QPushButton *DailyDataAdder::createAddButton(const QString &content)
{
    QPushButton *pAddButton = WidgetManager::createPushButton("추가");

    connect(pAddButton, &QPushButton::released, this, [&, content]()
    {
        QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();

        connect(pNetworkManager, &QNetworkAccessManager::finished,
                pNetworkManager, &QNetworkAccessManager::deleteLater);
        connect(pNetworkManager, &QNetworkAccessManager::finished,
                ApiManager::getInstance(), &ApiManager::handleStatusCode);

        if (content == "카오스던전")
        {
            ApiManager::getInstance()->postStatistics(pNetworkManager,
                                                      Statistics::Chaos,
                                                      mRewardChaos.data());
        }
        else if (content == "가디언토벌")
        {
            ApiManager::getInstance()->postStatistics(pNetworkManager,
                                                      Statistics::Guardian,
                                                      mRewardGuardian.data());
        }

        this->close();
    });

    return pAddButton;
}
