#include "tripod_search_item.h"
#include "ui_tripod_search_item.h"
#include "ui/widget_manager.h"
#include "game/item/item_grade.h"
#include "game/skill/skill_manager.h"

#include <QLabel>
#include <QGroupBox>

#include <QNetworkAccessManager>

TripodSearchItem::TripodSearchItem(const AuctionItem &item, const QString &className) :
    ui(new Ui::TripodSearchItem)
{
    ui->setupUi(this);

    ui->hLayoutMain->addLayout(createItemLayout(item.iconPath, item.itemName,
                                                item.itemGrade));
    ui->hLayoutMain->addLayout(createSkillLayout(item.itemOptions[0].optionName,
                                                 className));
    ui->hLayoutMain->addLayout(createTripodLayout(item.itemOptions[0].tripod,
                                                  item.itemOptions[0].optionName,
                                                  className,
                                                  item.itemOptions[0].value));
    ui->hLayoutMain->addWidget(createBidPriceGroup(item.nextBidPrice));
    ui->hLayoutMain->addWidget(createBuyPriceGroup(item.buyPrice));
}

TripodSearchItem::~TripodSearchItem()
{
    delete ui;
}

QVBoxLayout *TripodSearchItem::createItemLayout(const QString &iconPath, const QString &itemName, const QString &itemGrade)
{
    QVBoxLayout *pItemLayout = new QVBoxLayout();

    pItemLayout->addWidget(createItemIcon(iconPath), 0, Qt::AlignHCenter);
    pItemLayout->addWidget(createItemNameLabel(itemName, itemGrade),
                           0, Qt::AlignHCenter);

    return pItemLayout;
}

QLabel *TripodSearchItem::createItemIcon(const QString &iconPath)
{
    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished,
            pNetworkManager, &QNetworkAccessManager::deleteLater);

    QLabel *pIcon = WidgetManager::createIcon(iconPath, pNetworkManager);

    return pIcon;
}

QLabel *TripodSearchItem::createItemNameLabel(const QString &itemName, const QString &itemGrade)
{
    QLabel *pItemNameLabel = WidgetManager::createLabel(itemName);
    pItemNameLabel->setStyleSheet(QString("QLabel { color: %1 }").arg(
        itemGradeToTextColor(strToItemGrade(itemGrade))));

    return pItemNameLabel;
}

QVBoxLayout *TripodSearchItem::createSkillLayout(const QString &skillName, const QString &className)
{
    QVBoxLayout *pSkillLayout = new QVBoxLayout();

    pSkillLayout->addWidget(createSkillIcon(skillName, className),
                            0, Qt::AlignHCenter);
    pSkillLayout->addWidget(createSkillNameLabel(skillName), 0, Qt::AlignHCenter);

    return pSkillLayout;
}

QLabel *TripodSearchItem::createSkillIcon(const QString &skillName, const QString &className)
{
    QLabel *pSkillIcon = WidgetManager::createIcon(
        SkillManager::getInstance()->skill(className, skillName).iconPath, nullptr);

    return pSkillIcon;
}

QLabel *TripodSearchItem::createSkillNameLabel(const QString &skillName)
{
    QLabel *pSkillNameLabel = WidgetManager::createLabel(skillName);

    return pSkillNameLabel;
}

QVBoxLayout *TripodSearchItem::createTripodLayout(const QString &tripodName, const QString &skillName, const QString &className, int level)
{
    QVBoxLayout *pTripodLayout = new QVBoxLayout();

    pTripodLayout->addWidget(createTripodIcon(tripodName, skillName, className),
                             0, Qt::AlignHCenter);
    pTripodLayout->addWidget(createTripodNameLabel(tripodName, level),
                             0, Qt::AlignHCenter);

    return pTripodLayout;
}

QLabel *TripodSearchItem::createTripodIcon(const QString &tripodName, const QString &skillName, const QString &className)
{
    const QList<Tripod> &tripods = SkillManager::getInstance()->skill(
                                                                  className,
                                                                  skillName).tripods;
    QString iconPath;

    for (const Tripod &tripod : tripods)
    {
        if (tripod.tripodName == tripodName)
        {
            iconPath = tripod.iconPath;
            break;
        }
    }

    QLabel *pTripodIcon = WidgetManager::createIcon(iconPath, nullptr, "black");

    return pTripodIcon;
}

QLabel *TripodSearchItem::createTripodNameLabel(const QString &tripodName, int level)
{
    QLabel *pTripodNameLabel = WidgetManager::createLabel(
        QString("%1 Lv.%2").arg(tripodName).arg(level));

    return pTripodNameLabel;
}

QGroupBox *TripodSearchItem::createBidPriceGroup(int nextBidPrice)
{
    QGroupBox *pBidPriceGroup = WidgetManager::createGroupBox("최소 입찰가");

    QVBoxLayout *pBidPriceGroupLayout = new QVBoxLayout();
    pBidPriceGroup->setLayout(pBidPriceGroupLayout);

    QLabel *pBidPriceLabel = WidgetManager::createLabel(QString::number(nextBidPrice));
    pBidPriceGroupLayout->addWidget(pBidPriceLabel);

    return pBidPriceGroup;
}

QGroupBox *TripodSearchItem::createBuyPriceGroup(int buyPrice)
{
    QGroupBox *pBuyPriceGroup = WidgetManager::createGroupBox("즉시 구매가");

    QVBoxLayout *pBuyPriceGroupLayout = new QVBoxLayout();
    pBuyPriceGroup->setLayout(pBuyPriceGroupLayout);

    QString priceText = buyPrice == -1 ? "-" : QString::number(buyPrice);
    QLabel *pBuyPriceLabel = WidgetManager::createLabel(priceText);
    pBuyPriceGroupLayout->addWidget(pBuyPriceLabel);

    return pBuyPriceGroup;
}
