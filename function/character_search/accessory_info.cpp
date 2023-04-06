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
        ui->vLayout2->addWidget(pLabel);
        mWidgets << pLabel;
        return;
    }

    initializeLayout1(pAccessory);
    initializeLayout2(pAccessory);
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

void AccessoryInfo::initializeLayout1(const Accessory *pAccessory)
{
    addAccessoryIcon(pAccessory->iconPath(), pAccessory->itemGrade());
    addQualityBar(pAccessory->quality());
}

void AccessoryInfo::addAccessoryIcon(const QString &iconPath, ItemGrade itemGrade)
{
    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    QLabel *pIcon = WidgetManager::createIcon(iconPath, pNetworkManager, itemGradeToBGColor(itemGrade));
    ui->vLayout1->addWidget(pIcon);
    mWidgets << pIcon;
}

void AccessoryInfo::addQualityBar(int quality)
{
    QProgressBar *pQualityBar = WidgetManager::createQualityBar(quality, 50, 25);
    ui->vLayout1->addWidget(pQualityBar);
    mWidgets << pQualityBar;
}

void AccessoryInfo::initializeLayout2(const Accessory *pAccessory)
{
    addItemGradeInfo(pAccessory->itemGrade());
    addAbilityInfo(pAccessory->abilities());
    addEngraveInfo(pAccessory->getEngrave()->getEngraves(), pAccessory->getEngrave()->getPenalties(), pAccessory);
}

void AccessoryInfo::addItemGradeInfo(ItemGrade itemGrade)
{
    QHBoxLayout *pHLayout = createHLayout(ui->vLayout2);

    QLabel *pLabelItemGrade = WidgetManager::createLabel(itemGradeToQString(itemGrade), 10, "", 50);
    pLabelItemGrade->setStyleSheet(QString("QLabel { border: 1px solid black; "
                                           "         border-radius: 5px; "
                                           "         padding: 2px;"
                                           "         color: %1 }").arg(itemGradeToTextColor(itemGrade)));
    pHLayout->addWidget(pLabelItemGrade);
    mWidgets << pLabelItemGrade;
}

void AccessoryInfo::addAbilityInfo(const QHash<Ability, int> &abilities)
{
    QHBoxLayout *pHLayout = createHLayout(ui->vLayout2);

    for (auto iter = abilities.begin(); iter != abilities.end(); iter++)
    {
        const QString text = QString("%1 +%2").arg(abilityToQString(iter.key())).arg(iter.value());

        QLabel *pLabelAbility = WidgetManager::createLabel(text, 10, "", 75);
        pHLayout->addWidget(pLabelAbility);
        mWidgets << pLabelAbility;
    }
}

void AccessoryInfo::addEngraveInfo(const QStringList &engraves, const QStringList &penalties, const Accessory *pAccessory)
{
    const QList<QStringList> engraveList = {engraves, penalties};
    const QStringList textColor = {"#FFA500", "red"};

    for (int i = 0; i < 2; i++)
    {
        QHBoxLayout *pHLayout = createHLayout(ui->vLayout2);

        for (const QString &engrave : engraveList[i])
        {
            QString text = "%1 +%2";

            if (i == 0)
                text = text.arg(engrave).arg(pAccessory->getEngrave()->getEngraveValue(engrave));
            else if (i == 1)
                text = text.arg(engrave).arg(pAccessory->getEngrave()->getPenaltyValue(engrave));

            QLabel *pLabelEngrave = WidgetManager::createLabel(text, 10, textColor[i], 100);
            pHLayout->addWidget(pLabelEngrave);
            mWidgets << pLabelEngrave;
        }
    }
}

QHBoxLayout *AccessoryInfo::createHLayout(QVBoxLayout *pLayout)
{
    QHBoxLayout *pHLayout = new QHBoxLayout();

    pHLayout->setSpacing(5);
    pLayout->addLayout(pHLayout);
    pLayout->setAlignment(pHLayout, Qt::AlignLeft);
    mLayouts << pHLayout;

    return pHLayout;
}
