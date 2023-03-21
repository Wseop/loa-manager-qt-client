#include "accessory_info.h"
#include "ui_accessory_info.h"
#include "ui/widget_manager.h"
#include "game/item/accessory.h"

#include <QLabel>
#include <QProgressBar>
#include <QNetworkAccessManager>
#include <QNetworkReply>

AccessoryInfo::AccessoryInfo(const Accessory *pAccessory) :
    ui(new Ui::AccessoryInfo)
{
    ui->setupUi(this);

    if (pAccessory == nullptr)
    {
        QLabel *pLabel = WidgetManager::createLabel("악세 미착용");
        ui->vLayoutRight->addWidget(pLabel);
        mWidgets << pLabel;
        return;
    }

    initializeIcon(pAccessory->iconPath(), pAccessory->itemGrade());
    initializeQualityBar(pAccessory->quality());
    initializeItemGradeInfo(pAccessory->itemGrade());
    initializeAbilityInfo(pAccessory->abilities());
    initializeEngraveInfo(pAccessory->getEngraves(), pAccessory);
    initializePenaltyInfo(pAccessory->getPenalties(), pAccessory);
}

AccessoryInfo::~AccessoryInfo()
{
    for (QWidget *pWidget : mWidgets)
        delete pWidget;
    mWidgets.clear();

    for (auto rIter = mLayouts.rbegin(); rIter != mLayouts.rend(); rIter++)
        delete *rIter;
    mLayouts.clear();

    delete ui;
}

void AccessoryInfo::initializeIcon(const QString &iconPath, ItemGrade itemGrade)
{
    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    QLabel *pIcon = WidgetManager::createIcon(iconPath, pNetworkManager, itemGradeToBGColor(itemGrade));
    ui->vLayoutLeft->addWidget(pIcon);
    mWidgets << pIcon;
}

void AccessoryInfo::initializeQualityBar(int quality)
{
    QProgressBar *pQualityBar = WidgetManager::createQualityBar(quality, 50, 25);
    ui->vLayoutLeft->addWidget(pQualityBar);
    mWidgets << pQualityBar;
}

void AccessoryInfo::initializeItemGradeInfo(ItemGrade itemGrade)
{
    QHBoxLayout *pHLayout = createHLayout();

    QLabel *pLabelItemGrade = WidgetManager::createLabel(itemGradeToQString(itemGrade), 10, "", 50);
    pLabelItemGrade->setStyleSheet(QString("QLabel { border: 1px solid black; border-radius: 5px; padding: 2px;"
                                   "         color: %1 }").arg(itemGradeToTextColor(itemGrade)));
    pHLayout->addWidget(pLabelItemGrade);
    mWidgets << pLabelItemGrade;
}

void AccessoryInfo::initializeAbilityInfo(const QHash<Ability, int> &abilities)
{
    QHBoxLayout *pHLayout = createHLayout();

    for (auto iter = abilities.begin(); iter != abilities.end(); iter++)
    {
        QString text = QString("%1 +%2").arg(abilityToQString(iter.key())).arg(iter.value());

        QLabel *pLabelAbility = WidgetManager::createLabel(text, 10, "", 75);
        pHLayout->addWidget(pLabelAbility);
        mWidgets << pLabelAbility;
    }
}

void AccessoryInfo::initializeEngraveInfo(const QStringList &engraves, const Accessory *pAccessory)
{
    QHBoxLayout *pHLayout = createHLayout();

    for (const QString &engrave : engraves)
    {
        QString text = QString("%1 +%2").arg(engrave).arg(pAccessory->getEngraveValue(engrave));

        QLabel *pLabelEngrave = WidgetManager::createLabel(text, 10, "#FFA500", 100);
        pHLayout->addWidget(pLabelEngrave);
        mWidgets << pLabelEngrave;
    }
}

void AccessoryInfo::initializePenaltyInfo(const QStringList &penalties, const Accessory *pAccessory)
{
    QHBoxLayout *pHLayout = createHLayout();

    for (const QString &penalty : penalties)
    {
        QString text = QString("%1 +%2").arg(penalty).arg(pAccessory->getPenaltyValue(penalty));

        QLabel *pLabelPenalty = WidgetManager::createLabel(text, 10, "red", 100);
        pHLayout->addWidget(pLabelPenalty);
        mWidgets << pLabelPenalty;
    }
}

QHBoxLayout *AccessoryInfo::createHLayout()
{
    QHBoxLayout *pHLayout = new QHBoxLayout();

    pHLayout->setSpacing(5);
    ui->vLayoutRight->addLayout(pHLayout);
    ui->vLayoutRight->setAlignment(pHLayout, Qt::AlignLeft);
    mLayouts << pHLayout;

    return pHLayout;
}
