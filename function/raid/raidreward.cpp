#include "raidreward.h"
#include "ui_raidreward.h"
#include "ui/widget_manager.h"
#include "game_data/character/item/enum/item_grade.h"
#include "function/raid/raidreward_profit.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>

RaidReward::RaidReward(QString raid, QList<Reward> rewards, QMap<QString, QString> iconPaths) :
    ui(new Ui::RaidReward),
    m_iconPaths(iconPaths),
    m_itemTotalPrices(rewards.size())
{
    ui->setupUi(this);

    for (const Reward& reward : rewards)
    {
        QHBoxLayout* pHLayout = new QHBoxLayout();
        ui->vLayoutRaidReward->addLayout(pHLayout);
        m_layouts.append(pHLayout);

        addGateLabel(pHLayout, reward.gate);

        QGroupBox* pGroupBox = new QGroupBox();
        pHLayout->addWidget(pGroupBox);
        m_widgets.append(pGroupBox);
        QHBoxLayout* pGroupHLayout = new QHBoxLayout();
        pGroupBox->setLayout(pGroupHLayout);
        m_layouts.append(pGroupHLayout);
        addRewardLabel(pGroupHLayout, reward.items);

        addCostLabel(pHLayout, reward.cost);
    }
}

RaidReward::~RaidReward()
{
    for (QWidget* pWidget : m_widgets)
        delete pWidget;
    for (QLayout* pLayout : m_layouts)
        delete pLayout;

    m_itemPriceLabels.clear();
    m_profitLabels.clear();

    delete ui;
}

void RaidReward::updateItemPrice(QString item)
{
    QString priceText = "[%L1골]";

    if (m_itemPrices.contains(item))
    {
        m_itemPrices[item] = RaidRewardProfit::getInstance()->getItemPrice(item);

        for (int i = 0; i < m_itemPriceLabels.size(); i++)
        {
            double totalPrice = m_itemPrices[item] * m_itemCounts[i][item];
            m_itemPriceLabels[i][item]->setText(priceText.arg(totalPrice));
            m_itemTotalPrices[i][item] = totalPrice;
        }

        updateProfit();
    }
}

void RaidReward::addGateLabel(QHBoxLayout* pLayout, QString gate)
{
    QLabel* pLabelGate = WidgetManager::createLabel(gate, 12);
    m_widgets.append(pLabelGate);
    pLayout->addWidget(pLabelGate);
}

void RaidReward::addRewardLabel(QHBoxLayout* pLayout, QList<QPair<QString, int>> items)
{
    QMap<QString, int> itemCounts;
    QMap<QString, QLabel*> itemPriceLabels;

    for (auto& item : items)
    {
        QString itemName = item.first;
        int itemCount = item.second;
        m_itemPrices[itemName] = 0;
        itemCounts[itemName] = itemCount;

        QVBoxLayout* pVLayout = new QVBoxLayout();
        pLayout->addLayout(pVLayout);

        QHBoxLayout* pHLayout = new QHBoxLayout();
        pVLayout->addLayout(pHLayout);

        QLabel* pIcon = WidgetManager::createIcon(m_iconPaths[itemName], nullptr, backgroundColorCode(ItemGrade::일반));
        pHLayout->addWidget(pIcon);

        QLabel* pLabelCount = WidgetManager::createLabel(QString("x%1").arg(itemCount), 10, "", 50, 25);
        pLabelCount->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
        pHLayout->addWidget(pLabelCount);

        QLabel* pLabelPrice = WidgetManager::createLabel("[-]");
        pVLayout->addWidget(pLabelPrice);
        itemPriceLabels[itemName] = pLabelPrice;

        m_widgets.append(pIcon);
        m_widgets.append(pLabelCount);
        m_widgets.append(pLabelPrice);
        m_layouts.append(pVLayout);
        m_layouts.append(pHLayout);
    }

    m_itemCounts.append(itemCounts);
    m_itemPriceLabels.append(itemPriceLabels);
}

void RaidReward::addCostLabel(QHBoxLayout* pLayout, int cost)
{
    m_costs.append(cost);

    QVBoxLayout* pVLayout = new QVBoxLayout();
    pLayout->addLayout(pVLayout);

    QLabel* pLabelCost = WidgetManager::createLabel(QString("더보기 비용 : [%1골]").arg(cost), 10, "", 150);
    pVLayout->addWidget(pLabelCost);

    QLabel* pLabelProfit = WidgetManager::createLabel("[-]", 10, "", 150);
    m_profitLabels.append(pLabelProfit);
    pVLayout->addWidget(pLabelProfit);
    pVLayout->setAlignment(pLabelProfit, Qt::AlignHCenter);

    m_widgets.append(pLabelCost);
    m_widgets.append(pLabelProfit);
    m_layouts.append(pVLayout);
}

void RaidReward::updateProfit()
{
    QString labelStyle = "QLabel { color: %1 }";

    for (int i = 0; i < m_costs.size(); i++)
    {
        double cost = (double)m_costs[i];
        double totalPrice = 0;
        for (auto iter = m_itemTotalPrices[i].begin(); iter != m_itemTotalPrices[i].end(); iter++)
            totalPrice += iter.value();

        double diff = totalPrice - cost;
        if (diff >= 0)
        {
            m_profitLabels[i]->setText(QString("%L1골 이득").arg(diff, 0, 'f', 2));
            m_profitLabels[i]->setStyleSheet(labelStyle.arg("blue"));
        }
        else
        {
            m_profitLabels[i]->setText(QString("%L1골 손해").arg(-diff, 0, 'f', 2));
            m_profitLabels[i]->setStyleSheet(labelStyle.arg("red"));
        }
    }
}
