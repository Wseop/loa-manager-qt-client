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
    mContent(content),
    mContentLevels(contentLevels),
    mDropTable(dropTable)
{
    ui->setupUi(this);

    initializeUIContentLevel();
    initializeUIDropTable();
}

ContentRewardTable::~ContentRewardTable()
{
    delete ui;
}

void ContentRewardTable::refreshRewardData(const QHash<QString, Reward> newRewardData)
{
    const QStringList &levels = mCountLabel.keys();

    for (const QString &level : levels)
    {
        mRewardData[level] = newRewardData[level];

        int dataCount = mRewardData[level].count;

        if (dataCount == 0)
            continue;

        mCountLabel[level]->setText(QString::number(dataCount));

        const QList<int> &itemCounts = mRewardData[level].itemCounts;

        for (int i = 0; i < itemCounts.size(); i++)
        {
            mItemCountLabels[level][i]->setText(QString("%L1").arg(itemCounts[i] / (double)dataCount, 0, 'f', 2));
        }
    }
}

void ContentRewardTable::refreshTradablePrice(const QHash<QString, int> &newTradablePrice, const QHash<QString, QCheckBox*> &tradableSelector)
{
    // 아이템 시세 업데이트
    const QStringList &tradables = newTradablePrice.keys();

    for (const QString &tradable : tradables)
    {
        if (tradable == "명예의 파편 주머니(소)")
            mTradablePrice["명예의 파편"] = newTradablePrice[tradable] / (double)500;
        else if (tradable == "1레벨 멸화")
            mTradablePrice["보석"] = newTradablePrice[tradable];
        else if (tradable.contains("강석"))
            mTradablePrice[tradable] = newTradablePrice[tradable] / (double)10;
        else
            mTradablePrice[tradable] = newTradablePrice[tradable];
    }

    // UI 업데이트
    const QStringList &levels = mGoldLabel.keys();

    for (const QString &level : levels)
    {
        const QStringList &items = mDropTable[level];
        int dataCount = mRewardData[level].count;
        QList<int> &itemCounts = mRewardData[level].itemCounts;
        double totalGold = 0;

        for (int i = 0; i < items.size(); i++)
        {
            const QString &item = items[i];

            if (!mTradablePrice.contains(item))
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
                totalGold += (itemCounts[i] / (double)dataCount) * mTradablePrice[item];
        }

        mGoldLabel[level]->setText(QString("%L1").arg(totalGold, 0, 'f', 2));
    }
}

void ContentRewardTable::initializeUIContentLevel()
{
    int row = 0;

    QLabel *pLabel = WidgetManager::createLabel("단계", 12);
    ui->gridTable->addWidget(pLabel, row++, 0, Qt::AlignHCenter);

    QFrame *pHLine = WidgetManager::createLine(QFrame::HLine);
    ui->gridTable->addWidget(pHLine, row++, 0, 1, -1);

    for (int i = 0; i < mContentLevels.size(); i++)
    {
        QLabel *pLabelLevel = WidgetManager::createLabel(mContentLevels[i]);
        ui->gridTable->addWidget(pLabelLevel, row++, 0);

        QFrame *pHLine2 = WidgetManager::createLine(QFrame::HLine);
        ui->gridTable->addWidget(pHLine2, row++, 0, 1, -1);
    }

    QFrame *pVLine = WidgetManager::createLine(QFrame::VLine);
    ui->gridTable->addWidget(pVLine, 0, 1, -1, 1);
}

void ContentRewardTable::initializeUIDropTable()
{
    int row = 2;
    int col = 0;

    QLabel *pLabel = WidgetManager::createLabel("컨텐츠 보상 평균 획득량 (2수, 휴식X)", 12, 300);

    // 보상 평균 획득량 layout
    for (int i = 0; i < mContentLevels.size(); i++)
    {
        const QStringList &items = mDropTable[mContentLevels[i]];
        int itemCol = 2;

        for (const QString &item : items)
        {
            const QString &level = mContentLevels[i];

            QVBoxLayout *pVLayout = new QVBoxLayout();
            ui->gridTable->addLayout(pVLayout, row, itemCol++);

            const QString &iconPath = ResourceManager::getInstance()->iconPath(item);

            QLabel *pLabelItem = nullptr;
            if (iconPath != "")
                pLabelItem = WidgetManager::createIcon(iconPath, nullptr);
            else
                pLabelItem = WidgetManager::createLabel(item, 10, 50, 50);
            pVLayout->addWidget(pLabelItem);
            pVLayout->setAlignment(pLabelItem, Qt::AlignHCenter);

            QLabel *pLabelCount = WidgetManager::createLabel("-", 10, 100, 25);
            pLabelCount->setStyleSheet("QLabel { border: 1px solid black }");
            pVLayout->addWidget(pLabelCount);
            pVLayout->setAlignment(pLabelCount, Qt::AlignBottom);
            mItemCountLabels[level].append(pLabelCount);
        }
        row += 2;
        col = col > itemCol ? col : itemCol;
    }

    ui->gridTable->addWidget(pLabel, 0, 2, 1, col - 2, Qt::AlignHCenter);

    QFrame *pVLine = WidgetManager::createLine(QFrame::VLine);
    ui->gridTable->addWidget(pVLine, 0, col++, -1, 1);

    initializeUIGold(col);
}

void ContentRewardTable::initializeUIGold(int col)
{
    int row = 0;

    // 골드 가치 layout
    QLabel *pLabelGoldAttr = WidgetManager::createLabel("골드 가치", 12);
    ui->gridTable->addWidget(pLabelGoldAttr, row++, col);

    row++;

    for (int i = 0; i < mContentLevels.size(); i++)
    {
        const QString &level = mContentLevels[i];

        QLabel *pLabelGold = WidgetManager::createLabel("-");
        ui->gridTable->addWidget(pLabelGold, row++, col);
        mGoldLabel[level] = pLabelGold;

        row++;
    }

    QFrame *pVLine = WidgetManager::createLine(QFrame::VLine);
    ui->gridTable->addWidget(pVLine, 0, ++col, -1, 1);

    // 누적 데이터 수 layout
    row = 0;

    QLabel *pLabelCountAttr = WidgetManager::createLabel("누적 데이터 수", 12);
    ui->gridTable->addWidget(pLabelCountAttr, row++, ++col);

    row++;

    for (int i = 0; i < mContentLevels.size(); i++)
    {
        const QString &level = mContentLevels[i];

        QLabel *pLabelCount = WidgetManager::createLabel("-");
        ui->gridTable->addWidget(pLabelCount, row++, col);
        mCountLabel[level] = pLabelCount;

        row++;
    }
}
