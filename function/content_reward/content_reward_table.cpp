#include "content_reward_table.h"
#include "ui_content_reward_table.h"
#include "ui/widget_manager.h"
#include "resource/resource_manager.h"

#include <QLabel>
#include <QFrame>
#include <QCheckBox>
#include <QVBoxLayout>

ContentRewardTable::ContentRewardTable(const QString &content, const QStringList &contentLevels, const QHash<QString, QStringList> &dropTable) :
    ui(new Ui::ContentRewardTable),
    m_content(content),
    m_contentLevels(contentLevels),
    m_dropTable(dropTable)
{
    ui->setupUi(this);

    if (m_content == "카오스 던전")
    {
        m_stages = {2, 2, 2, 1};
    }
    else
    {
        for (int i = 0; i < m_contentLevels.size(); i++)
            m_stages.append(1);
    }

    initializeUIContentLevel();
    initializeUIDropTable();
}

ContentRewardTable::~ContentRewardTable()
{
    m_itemCountLabels.clear();
    m_goldLabel.clear();
    m_countLabel.clear();

    for (QWidget *pWidget : m_widgets)
        delete pWidget;
    m_widgets.clear();

    for (QLayout *pLayout : m_layouts)
        delete pLayout;
    m_layouts.clear();

    delete ui;
}

void ContentRewardTable::refreshRewardData(const QHash<QString, RewardData> &newRewardData)
{
    const QStringList &levels = m_countLabel.keys();
    for (const QString &level : levels)
    {
        m_rewardData[level] = newRewardData[level];

        int dataCount = m_rewardData[level].dataCount;
        if (dataCount == 0)
            continue;

        m_countLabel[level]->setText(QString::number(dataCount));

        const QList<int> &itemCounts = m_rewardData[level].itemCounts;
        for (int i = 0; i < itemCounts.size(); i++)
            m_itemCountLabels[level][i]->setText(QString("%L1").arg(itemCounts[i] / (double)dataCount, 0, 'f', 2));
    }
}

void ContentRewardTable::refreshTradablePrice(const QHash<QString, int> &newTradablePrice, const QHash<QString, QCheckBox*> &tradableSelector)
{
    // 아이템 시세 업데이트
    const QStringList &tradables = newTradablePrice.keys();
    for (const QString &tradable : tradables)
    {
        if (tradable == "명예의 파편 주머니(소)")
            m_tradablePrice["명예의 파편"] = newTradablePrice[tradable] / (double)500;
        else if (tradable == "1레벨 멸화")
            m_tradablePrice["보석"] = newTradablePrice[tradable];
        else if (tradable.contains("강석"))
            m_tradablePrice[tradable] = newTradablePrice[tradable] / (double)10;
        else
            m_tradablePrice[tradable] = newTradablePrice[tradable];
    }

    // UI 업데이트
    const QStringList &levels = m_goldLabel.keys();
    for (const QString &level : levels)
    {
        const QString dropTableKey = m_content == "카오스 던전" ? level.chopped(1) : level;
        const QStringList &items = m_dropTable[dropTableKey];
        int dataCount = m_rewardData[level].dataCount;
        QList<int> &itemCounts = m_rewardData[level].itemCounts;
        double totalGold = 0;

        for (int i = 0; i < items.size(); i++)
        {
            const QString &item = items[i];
            if (!m_tradablePrice.contains(item))
                continue;

            bool selected = false;
            if (item == "명예의 파편" || item == "보석")
                selected = tradableSelector[item]->isChecked();
            else if (item.contains("파괴강석"))
                selected = tradableSelector["파괴석"]->isChecked();
            else if (item.contains("수호강석"))
                selected = tradableSelector["수호석"]->isChecked();
            else if (item.contains("돌파석"))
                selected = tradableSelector["돌파석"]->isChecked();

            if (selected)
                totalGold += (itemCounts[i] / (double)dataCount) * m_tradablePrice[item];
        }

        m_goldLabel[level]->setText(QString("%L1").arg(totalGold, 0, 'f', 2));
    }
}

void ContentRewardTable::initializeUIContentLevel()
{
    int row = 0;

    QLabel *pLabel = WidgetManager::createLabel("단계", 12);
    ui->gridTable->addWidget(pLabel, row++, 0, Qt::AlignHCenter);
    m_widgets.append(pLabel);

    QFrame *pHLine = WidgetManager::createLine(QFrame::HLine);
    ui->gridTable->addWidget(pHLine, row++, 0, 1, -1);
    m_widgets.append(pHLine);

    for (int i = 0; i < m_contentLevels.size(); i++)
    {
        for (int j = 0; j < m_stages[i]; j++)
        {
            QLabel *pLabelLevel = nullptr;

            if (m_content == "카오스 던전")
                pLabelLevel = WidgetManager::createLabel(m_contentLevels[i] + QString::number(j + 1));
            else
                pLabelLevel = WidgetManager::createLabel(m_contentLevels[i]);

            ui->gridTable->addWidget(pLabelLevel, row++, 0);
            m_widgets.append(pLabelLevel);

            QFrame *pHLine2 = WidgetManager::createLine(QFrame::HLine);
            ui->gridTable->addWidget(pHLine2, row++, 0, 1, -1);
            m_widgets.append(pHLine2);
        }
    }

    QFrame *pVLine = WidgetManager::createLine(QFrame::VLine);
    ui->gridTable->addWidget(pVLine, 0, 1, -1, 1);
    m_widgets.append(pVLine);
}

void ContentRewardTable::initializeUIDropTable()
{
    int row = 2;
    int col = 0;

    QLabel *pLabel = WidgetManager::createLabel("컨텐츠 보상 평균 획득량", 12);
    m_widgets.append(pLabel);

    // 보상 평균 획득량 layout
    for (int i = 0; i < m_contentLevels.size(); i++)
    {
        for (int j = 0; j < m_stages[i]; j++)
        {
            const QStringList &items = m_dropTable[m_contentLevels[i]];
            int itemCol = 2;

            for (const QString &item : items)
            {
                QString level = m_contentLevels[i];
                if (m_content == "카오스 던전")
                    level += QString::number(j + 1);

                QVBoxLayout *pVLayout = new QVBoxLayout();
                ui->gridTable->addLayout(pVLayout, row, itemCol++);
                m_layouts.append(pVLayout);

                QString iconPath = ResourceManager::getInstance()->iconPath(item);
                QLabel *pLabelItem = nullptr;
                if (iconPath != "")
                    pLabelItem = WidgetManager::createIcon(iconPath, nullptr);
                else
                    pLabelItem = WidgetManager::createLabel(item, 10, "", 50, 50);
                pVLayout->addWidget(pLabelItem);
                pVLayout->setAlignment(pLabelItem, Qt::AlignHCenter);
                m_widgets.append(pLabelItem);

                QLabel *pLabelCount = WidgetManager::createLabel("-", 10, "", 100, 25);
                pLabelCount->setStyleSheet("QLabel { border: 1px solid black }");
                pVLayout->addWidget(pLabelCount);
                pVLayout->setAlignment(pLabelCount, Qt::AlignBottom);
                m_itemCountLabels[level].append(pLabelCount);
                m_widgets.append(pLabelCount);
            }
            row += 2;
            col = col > itemCol ? col : itemCol;
        }
    }

    ui->gridTable->addWidget(pLabel, 0, 2, 1, col - 2, Qt::AlignHCenter);

    QFrame *pVLine = WidgetManager::createLine(QFrame::VLine);
    ui->gridTable->addWidget(pVLine, 0, col++, -1, 1);
    m_widgets.append(pVLine);

    initializeUIGold(col);
}

void ContentRewardTable::initializeUIGold(int col)
{
    int row = 0;

    // 골드 가치 layout
    QLabel *pLabelGoldAttr = WidgetManager::createLabel("골드 가치", 12);
    ui->gridTable->addWidget(pLabelGoldAttr, row++, col);
    m_widgets.append(pLabelGoldAttr);

    row++;

    for (int i = 0; i < m_contentLevels.size(); i++)
    {
        for (int j = 0; j < m_stages[i]; j++)
        {
            QString level = m_contentLevels[i];
            if (m_content == "카오스 던전")
                level += QString::number(j + 1);

            QLabel *pLabelGold = WidgetManager::createLabel("-");
            ui->gridTable->addWidget(pLabelGold, row++, col);
            m_goldLabel[level] = pLabelGold;
            m_widgets.append(pLabelGold);

            row++;
        }
    }

    QFrame *pVLine = WidgetManager::createLine(QFrame::VLine);
    ui->gridTable->addWidget(pVLine, 0, ++col, -1, 1);
    m_widgets.append(pVLine);

    // 누적 데이터 수 layout
    row = 0;

    QLabel *pLabelCountAttr = WidgetManager::createLabel("누적 데이터 수", 12);
    ui->gridTable->addWidget(pLabelCountAttr, row++, ++col);
    m_widgets.append(pLabelCountAttr);

    row++;

    for (int i = 0; i < m_contentLevels.size(); i++)
    {
        for (int j = 0; j < m_stages[i]; j++)
        {
            QString level = m_contentLevels[i];
            if (m_content == "카오스 던전")
                level += QString::number(j + 1);

            QLabel *pLabelCount = WidgetManager::createLabel("-");
            ui->gridTable->addWidget(pLabelCount, row++, col);
            m_countLabel[level] = pLabelCount;
            m_widgets.append(pLabelCount);

            row++;
        }
    }
}
