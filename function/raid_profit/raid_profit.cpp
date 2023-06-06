#include "raid_profit.h"
#include "ui_raid_profit.h"
#include "ui/widget_manager.h"
#include "api/api_manager.h"
#include "api/resources/reward.h"
#include "function/raid_profit/raid_profit_widget.h"

#include <QGroupBox>
#include <QComboBox>
#include <QLabel>
#include <QCheckBox>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

RaidProfit *RaidProfit::pInstance = nullptr;

RaidProfit::RaidProfit() :
    ui(new Ui::RaidProfit)
{
    ui->setupUi(this);

    initializeContent();
    initializeItem();

    loadResource();

    initializeInputLayout();
}

RaidProfit::~RaidProfit()
{
    delete ui;
}

void RaidProfit::initializeContent()
{
    mContents = {
        "발탄(노말)", "발탄(하드)",
        "비아키스(노말)", "비아키스(하드)",
        "쿠크세이튼",
        "아브렐슈드(노말)", "아브렐슈드(하드)",
        "카양겔(노말)", "카양겔(하드)",
        "일리아칸(노말)", "일리아칸(하드)",
        "혼돈의 상아탑(노말)", "혼돈의 상아탑(하드)"
    };
}

void RaidProfit::initializeItem()
{
    QStringList items = {
        "명예의 파편", "파괴석", "수호석", "돌파석", "정수"
    };

    for (const QString &item : items)
        mSelectedItems[item] = true;
}

void RaidProfit::loadResource()
{
    for (const QString &content : mContents)
    {
        QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();

        connect(pNetworkManager, &QNetworkAccessManager::finished,
                pNetworkManager, &QNetworkAccessManager::deleteLater);
        connect(pNetworkManager, &QNetworkAccessManager::finished,
                this, [&](QNetworkReply *pReply)
        {
            if (!ApiManager::getInstance()->handleStatusCode(pReply))
                return;

            QJsonObject resource = QJsonDocument::fromJson(pReply->readAll()).object();

            ContentReward contentReward;
            contentReward.content = resource.find("content")->toString();

            const QJsonArray &rewards = resource.find("rewards")->toArray();
            for (auto iterReward = rewards.constBegin(); iterReward != rewards.constEnd(); iterReward++)
            {
                Reward reward;
                reward.level = iterReward->toObject().find("level")->toString();
                reward.cost = iterReward->toObject().find("cost")->toInt();

                const QJsonArray &rewardItems = iterReward->toObject()
                                                    .find("items")->toArray();
                for (auto iterItem = rewardItems.constBegin(); iterItem != rewardItems.constEnd(); iterItem++)
                {
                    RewardItem rewardItem;
                    rewardItem.item = iterItem->toObject().find("item")->toString();
                    rewardItem.count = iterItem->toObject().find("count")->toInt();
                    reward.items << rewardItem;
                }

                contentReward.rewards << reward;
            }

            RaidProfitWidget *pRaidProfitWidget = new RaidProfitWidget(contentReward);

            ui->vLayoutOutput->addWidget(pRaidProfitWidget);
            mRaidProfitWidgetMap[contentReward.content] = pRaidProfitWidget;

            if (contentReward.content == "발탄(노말)")
                pRaidProfitWidget->show();
            else
                pRaidProfitWidget->hide();
        });

        ApiManager::getInstance()->getResources(
            pNetworkManager, Resources::Reward, content);
    }
}

void RaidProfit::initializeInputLayout()
{
    ui->hLayoutInput->setAlignment(Qt::AlignHCenter);
    ui->hLayoutInput->addWidget(createContentSelectorGroup());
    ui->hLayoutInput->addWidget(createItemSelectorGroup());
}

QGroupBox *RaidProfit::createContentSelectorGroup()
{
    QGroupBox *pContentSelectorGroup = WidgetManager::createGroupBox("레이드 선택");

    QVBoxLayout *pContentSelectorLayout = new QVBoxLayout();
    pContentSelectorGroup->setLayout(pContentSelectorLayout);

    QComboBox *pContentSelector = WidgetManager::createComboBox(mContents);
    pContentSelectorLayout->addWidget(pContentSelector);

    connect(pContentSelector, &QComboBox::currentTextChanged,
            this, &RaidProfit::showSelectedContent);

    return pContentSelectorGroup;
}

QGroupBox *RaidProfit::createItemSelectorGroup()
{
    QGroupBox *pItemSelectorGroup = WidgetManager::createGroupBox("손익 계산에 포함");

    QHBoxLayout *pItemSelectorGroupLayout = new QHBoxLayout();
    pItemSelectorGroupLayout->setSpacing(15);
    pItemSelectorGroup->setLayout(pItemSelectorGroupLayout);

    QStringList itemNames = {
        "명예의 파편", "파괴석", "수호석", "돌파석", "정수"
    };

    for (const QString &itemName : itemNames)
        pItemSelectorGroupLayout->addLayout(createItemSelector(itemName));

    return pItemSelectorGroup;
}

QVBoxLayout *RaidProfit::createItemSelector(const QString &itemName)
{
    QVBoxLayout *pItemSelectorLayout = new QVBoxLayout();

    QLabel *pItemSelectorLabel = WidgetManager::createLabel(itemName);
    pItemSelectorLayout->addWidget(pItemSelectorLabel, 0, Qt::AlignHCenter);

    QCheckBox *pItemSelector = new QCheckBox();
    pItemSelector->setCheckState(Qt::Checked);
    pItemSelectorLayout->addWidget(pItemSelector, 0, Qt::AlignHCenter);

    connect(pItemSelector, &QCheckBox::stateChanged, this, [&, itemName](int state)
    {
        if (state == Qt::Checked)
            mSelectedItems[itemName] = true;
        else
            mSelectedItems[itemName] = false;

        refresh();
    });

    return pItemSelectorLayout;
}

void RaidProfit::initializeOutputLayout()
{
}

void RaidProfit::showSelectedContent(const QString &content)
{
    for (auto iter = mRaidProfitWidgetMap.constBegin(); iter != mRaidProfitWidgetMap.constEnd(); iter++)
    {
        if (iter.key() == content)
            iter.value()->show();
        else
            iter.value()->hide();
    }
}

void RaidProfit::refresh()
{
    QStringList selectedItems;

    for (auto iter = mSelectedItems.constBegin(); iter != mSelectedItems.constEnd(); iter++)
    {
        if (iter.value())
            selectedItems << iter.key();
    }

    for (auto iter = mRaidProfitWidgetMap.constBegin(); iter != mRaidProfitWidgetMap.constEnd(); iter++)
    {
        iter.value()->updateProfit(selectedItems);
    }
}

RaidProfit *RaidProfit::getInstance()
{
    if (pInstance == nullptr)
        pInstance = new RaidProfit();

    return pInstance;
}

void RaidProfit::destroyInstance()
{
    if (pInstance == nullptr)
        return;

    delete pInstance;
    pInstance = nullptr;
}
