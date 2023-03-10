#include "raid_profit_table.h"
#include "ui_raid_profit_table.h"
#include "ui/widget_manager.h"
#include "resource/resource_manager.h"

#include <QLabel>
#include <QFrame>
#include <QCheckBox>

RaidProfitTable::RaidProfitTable(const QString &content, const QList<int> &costs, const QList<RewardItems> &rewardItems) :
    ui(new Ui::RaidProfitTable),
    m_content(content),
    m_costs(costs),
    m_rewardItems(rewardItems)
{
    ui->setupUi(this);

    initializeTable();
}

RaidProfitTable::~RaidProfitTable()
{
    for (QWidget *pWidget : m_widgets)
        delete pWidget;
    m_widgets.clear();

    for (QLayout *pLayout : m_layouts)
        delete pLayout;
    m_layouts.clear();

    delete ui;
}

void RaidProfitTable::refreshItemPrice(const QHash<QString, double> &itemPrice, const QList<QCheckBox*> &itemSelectors)
{
    for (int i = 0; i < m_costs.size(); i++)
    {
        const RewardItems &rewardItems = m_rewardItems[i];
        double totalGold = 0;
        double profit = 0;

        for (int j = 0; j < rewardItems.size(); j++)
        {
            const QString &itemName = rewardItems[j].first;
            int itemCount = rewardItems[j].second;
            double price = itemPrice[itemName];

            if (itemSelectors[j]->isChecked())
                totalGold += price * itemCount;
        }

        m_goldLabels[i]->setText(QString("%L1").arg(totalGold, 0, 'f', 2));

        profit = totalGold - m_costs[i];
        m_profitLabels[i]->setText(QString("%L1").arg(profit, 0, 'f', 2));

        if (profit > 0)
            m_profitLabels[i]->setStyleSheet("QLabel { color: blue }");
        else
            m_profitLabels[i]->setStyleSheet("QLabel { color: red }");
    }
}

void RaidProfitTable::initializeTable()
{
    QList<int> colSpans = {1, static_cast<int>(m_rewardItems[0].size()), 1, 1, 1};

    initializeTableColumn(colSpans);
    initializeTableRow(colSpans);
    initializeTableReward();
}

void RaidProfitTable::initializeTableColumn(const QList<int> &colSpans)
{
    int col = 0;

    const QStringList attributes = {"관문", "더보기 보상 목록", "더보기 비용", "골드 가치 합계", "더보기 손익"};
    for (int i = 0; i < attributes.size(); i++)
    {
        QLabel *pLabel = WidgetManager::createLabel(attributes[i], 12, "", 150);
        ui->gridTable->addWidget(pLabel, 0, col, 1, colSpans[i], Qt::AlignHCenter);
        m_widgets << pLabel;

        col += colSpans[i];

        if (i < attributes.size() - 1)
        {
            QFrame *pVLine = WidgetManager::createLine(QFrame::VLine);
            ui->gridTable->addWidget(pVLine, 0, col++, -1, 1, Qt::AlignHCenter);
            m_widgets << pVLine;
        }
    }
}

void RaidProfitTable::initializeTableRow(const QList<int> &colSpans)
{
    int gateSize = m_costs.size();
    int row = 1;
    QStringList gateNameOfKayangel = {"천공의 문 넬라시아", "영원한 빛의 요람"};

    for (int i = 0; i < gateSize; i++)
    {
        int col = 0;

        QFrame *pHLine = WidgetManager::createLine(QFrame::HLine);
        ui->gridTable->addWidget(pHLine, row++, 0, 1, -1);
        m_widgets << pHLine;

        // 관문
        QLabel *pLabelGate = nullptr;

        if (m_content.contains("카양겔"))
            pLabelGate = WidgetManager::createLabel(gateNameOfKayangel[i], 10, "", 100);
        else
            pLabelGate = WidgetManager::createLabel(QString("%1관문").arg(i + 1), 10, "", 100);

        ui->gridTable->addWidget(pLabelGate, row, col, Qt::AlignHCenter);
        m_widgets << pLabelGate;

        col += colSpans[0] + 1 + colSpans[1] + 1;

        // 더보기 비용
        QLabel *pLabelCost = WidgetManager::createLabel(QString("%L1").arg(m_costs[i]), 10, "", 100);
        ui->gridTable->addWidget(pLabelCost, row, col, Qt::AlignHCenter);
        m_widgets << pLabelCost;

        col += colSpans[2] + 1;

        // 골드 가치 합계
        QLabel *pLabelTotalGold = WidgetManager::createLabel("-", 10, "", 100);
        ui->gridTable->addWidget(pLabelTotalGold, row, col, Qt::AlignHCenter);
        m_goldLabels << pLabelTotalGold;
        m_widgets << pLabelTotalGold;

        col += colSpans[3] + 1;

        // 더보기 손익
        QLabel *pLabelProfit = WidgetManager::createLabel("-", 10, "", 100);
        ui->gridTable->addWidget(pLabelProfit, row++, col, Qt::AlignHCenter);
        m_profitLabels << pLabelProfit;
        m_widgets << pLabelProfit;
    }
}

void RaidProfitTable::initializeTableReward()
{
    int row = 2;

    for (const RewardItems &rewardItems : m_rewardItems)
    {
        int col = 2;

        for (const QPair<QString, int> &rewardItem : rewardItems)
        {
            QVBoxLayout *pLayoutItem = new QVBoxLayout();
            ui->gridTable->addLayout(pLayoutItem, row, col++, Qt::AlignHCenter);
            m_layouts << pLayoutItem;

            QLabel *pIconItem = WidgetManager::createIcon(ResourceManager::getInstance()->iconPath(rewardItem.first), nullptr);
            pLayoutItem->addWidget(pIconItem);
            m_widgets << pIconItem;

            QLabel *pLabelItemCount = WidgetManager::createLabel(QString("%L1").arg(rewardItem.second), 10, "", 100);
            pLayoutItem->addWidget(pLabelItemCount);
            m_widgets << pLabelItemCount;
        }

        row += 2;
    }
}
