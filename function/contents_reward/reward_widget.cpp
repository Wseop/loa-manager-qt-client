#include "reward_widget.h"
#include "ui_reward_widget.h"
#include "ui/widget_manager.h"
#include "game_data/character/item/enum/item_grade.h"
#include "resource/resource_manager.h"

#include <QLabel>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

RewardWidget::RewardWidget(ContentType type, QString level, int levelCount, QStringList items, QList<QJsonObject> data) :
    ui(new Ui::RewardWidget),
    m_type(type),
    m_level(level),
    m_levelCount(levelCount),
    m_items(items)
{
    ui->setupUi(this);

    initIconPath();
    initTitle();
    initData(data);
}

RewardWidget::~RewardWidget()
{
    for (QWidget* pWidget : m_widgets)
        delete pWidget;
    delete ui;
}

void RewardWidget::updatePrice(QString item, double price)
{
    m_itemPrices[item] = price;

    for (int i = 0; i < m_levelCount; i++)
    {
        QString level = m_level;
        if (m_type == ContentType::Chaos)
            level += QString::number(i + 1);

        int dataCount = m_dataCount[level];
        if (dataCount == 0)
            continue;

        const QList<int>& itemCounts = m_data[level];
        double totalPrice = 0;
        for (int j = 0; j < m_items.size(); j++)
        {
            if (!m_itemPrices.contains(m_items[j]))
                continue;

            double itemCountAvg = itemCounts[j] / dataCount;
            totalPrice += itemCountAvg * m_itemPrices[m_items[j]];
        }
        m_goldLabels[level]->setText(QString("%L1").arg(totalPrice, 0, 'f', 2));
    }
}

void RewardWidget::initIconPath()
{
    const QJsonObject json = ResourceManager::getInstance()->loadJson("reforge");

    QString iconPath = ":/reforge/image/reforge/reforge_%1_%2.png";
    QJsonArray reforges = json.find("Reforge")->toArray();
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
    m_iconPaths["골드"] = ":/money/image/money/money_1.png";
}

void RewardWidget::initTitle()
{
    int col = 0;

    QLabel* pLabelLevel = WidgetManager::createLabel("단계");
    ui->gridMain->addWidget(pLabelLevel, 0, col++);
    m_widgets.append(pLabelLevel);

    for (const QString& item : m_items)
    {
        QLabel* pLabelItem = nullptr;

        if (item == "전설" || item == "유물" || item == "고대")
            pLabelItem = WidgetManager::createLabel(item + "\n악세", 10, colorCode(strToItemGrade(item)), 50, 50);
        else
            pLabelItem = WidgetManager::createIcon(m_iconPaths[item], nullptr);

        ui->gridMain->addWidget(pLabelItem, 0, col++);
        ui->gridMain->setAlignment(pLabelItem, Qt::AlignHCenter);
        m_widgets.append(pLabelItem);
    }

    QLabel* pLabelGold = WidgetManager::createIcon(m_iconPaths["골드"], nullptr);
    ui->gridMain->addWidget(pLabelGold, 0, col++);
    ui->gridMain->setAlignment(pLabelGold, Qt::AlignHCenter);
    m_widgets.append(pLabelGold);

    QLabel* pLabelDataCount = WidgetManager::createLabel("누적 데이터 수");
    ui->gridMain->addWidget(pLabelDataCount, 0, col++);
    m_widgets.append(pLabelDataCount);
}

void RewardWidget::initData(const QList<QJsonObject>& data)
{
    // init m_data
    for (int i = 0; i < m_levelCount; i++)
    {
        QString key = m_level;
        if (m_type == ContentType::Chaos)
            key += QString::number(i + 1);

        m_data[key] = QList<int>(m_items.size(), 0);
        m_dataCount[key] = 0;
    }

    // 아이템별 누적값 업데이트
    for (int i = 0; i < data.size(); i++)
    {
        const QJsonObject& obj = data[i];

        const QString& key = obj.find("Level")->toString();
        for (int j = 0; j < m_items.size(); j++)
        {
            m_data[key][j] += obj.find(m_items[j])->toInt();
        }
        m_dataCount[key]++;
    }

    // 위젯 추가
    for (int i = 0; i < m_levelCount; i++)
    {
        QString key = m_level;
        if (m_type == ContentType::Chaos)
            key += QString::number(i + 1);

        QLabel* pLabelLevel = WidgetManager::createLabel(key);
        ui->gridMain->addWidget(pLabelLevel, i + 1, 0);
        m_widgets.append(pLabelLevel);

        const QList<int> itemCounts = m_data[key];
        int total = m_dataCount[key];
        for (int j = 0; j < itemCounts.size(); j++)
        {
            QString itemCountText;
            if (total == 0)
                itemCountText = "-";
            else
                itemCountText = QString("%L1").arg(itemCounts[j] / (double)total, 0, 'f', 2);

            QLabel* pLabelItemCount = WidgetManager::createLabel(itemCountText, 10, "", 100);
            ui->gridMain->addWidget(pLabelItemCount, i + 1, j + 1);
            m_widgets.append(pLabelItemCount);
        }

        QLabel* pLabelGold = WidgetManager::createLabel("-", 10, "", 100);
        ui->gridMain->addWidget(pLabelGold, i + 1, itemCounts.size() + 1);
        m_goldLabels[key] = pLabelGold;
        m_widgets.append(pLabelGold);

        QLabel* pLabelTotal = WidgetManager::createLabel(QString::number(total));
        ui->gridMain->addWidget(pLabelTotal, i + 1, itemCounts.size() + 2);
        m_widgets.append(pLabelTotal);
    }
}
