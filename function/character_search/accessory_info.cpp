#include "accessory_info.h"
#include "ui_accessory_info.h"
#include "ui/widget_manager.h"

#include <QLabel>
#include <QProgressBar>
#include <QNetworkAccessManager>
#include <QNetworkReply>

AccessoryInfo::AccessoryInfo(const Equipment &accessory) :
    ui(new Ui::AccessoryInfo)
{
    ui->setupUi(this);

    if (accessory.type == "")
    {
        QLabel *pLabel = WidgetManager::createLabel("악세 미착용");
        ui->vLayout2->addWidget(pLabel);
        return;
    }

    initializeLayout1(accessory.iconPath, accessory.itemGrade, accessory.quality);
    initializeLayout2(accessory.itemGrade, accessory.abilities, accessory.engraves);
}

AccessoryInfo::~AccessoryInfo()
{
    delete ui;
}

void AccessoryInfo::initializeLayout1(const QString &iconPath, const ItemGrade &itemGrade, int quality)
{
    addAccessoryIcon(iconPath, itemGrade);
    addQualityBar(quality);
}

void AccessoryInfo::addAccessoryIcon(const QString &iconPath, ItemGrade itemGrade)
{
    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished,
            pNetworkManager, &QNetworkAccessManager::deleteLater);

    QLabel *pIcon = WidgetManager::createIcon(
        iconPath, pNetworkManager, itemGradeToBGColor(itemGrade));
    ui->vLayout1->addWidget(pIcon);
}

void AccessoryInfo::addQualityBar(int quality)
{
    QProgressBar *pQualityBar = WidgetManager::createQualityBar(quality, 50, 25);
    ui->vLayout1->addWidget(pQualityBar);
}

void AccessoryInfo::initializeLayout2(const ItemGrade &itemGrade, const QHash<QString, int> &abilities, const QHash<QString, int> &engraves)
{
    addItemGradeInfo(itemGrade);
    addAbilityInfo(abilities);
    addEngraveInfo(engraves);
}

void AccessoryInfo::addItemGradeInfo(const ItemGrade &itemGrade)
{
    QHBoxLayout *pHLayout = createHLayout(ui->vLayout2);

    QLabel *pLabelItemGrade = WidgetManager::createLabel(
        itemGradeToStr(itemGrade), 10, 50);

    pLabelItemGrade->setStyleSheet(
        QString("QLabel { border: 1px solid black; "
                "         border-radius: 5px; "
                "         padding: 2px;"
                "         color: %1 }").arg(itemGradeToTextColor(itemGrade)));

    pHLayout->addWidget(pLabelItemGrade);
}

void AccessoryInfo::addAbilityInfo(const QHash<QString, int> &abilities)
{
    const QStringList &abilityNames = abilities.keys();
    const QString infoText = "%1 +%2";

    QHBoxLayout *pHLayout = createHLayout(ui->vLayout2);

    for (const QString &abilityName : abilityNames) {
        QLabel *pLabelAbilityInfo = WidgetManager::createLabel(
            infoText.arg(abilityName).arg(abilities[abilityName]),
            10, 75);

        pHLayout->addWidget(pLabelAbilityInfo);
    }
}

void AccessoryInfo::addEngraveInfo(const QHash<QString, int> &engraves)
{
    const QStringList &engraveNames = engraves.keys();
    const QString infoText = "%1 +%2";
    const QString infoStyle = "QLabel { color: %1 }";

    for (const QString &engraveName : engraveNames) {
        QHBoxLayout *pHLayout = createHLayout(ui->vLayout2);
        QLabel *pLabelEngraveInfo = WidgetManager::createLabel(
            infoText.arg(engraveName).arg(engraves[engraveName]), 10, 100);

        if (engraveName.contains("감소")) {
            pLabelEngraveInfo->setStyleSheet(infoStyle.arg("red"));
        } else {
            pLabelEngraveInfo->setStyleSheet(infoStyle.arg("#FFA500"));
        }

        pHLayout->addWidget(pLabelEngraveInfo);
    }
}

QHBoxLayout *AccessoryInfo::createHLayout(QVBoxLayout *pLayout)
{
    QHBoxLayout *pHLayout = new QHBoxLayout();

    pHLayout->setSpacing(5);
    pLayout->addLayout(pHLayout);
    pLayout->setAlignment(pHLayout, Qt::AlignLeft);

    return pHLayout;
}
