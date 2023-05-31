#include "abilitystone_info.h"
#include "ui_abilitystone_info.h"
#include "ui/widget_manager.h"

#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkReply>

AbilityStoneInfo::AbilityStoneInfo(const Equipment &abilityStone) :
    ui(new Ui::AbilityStoneInfo)
{
    ui->setupUi(this);

    if (abilityStone.type == "")
    {
        QLabel *pLabel = WidgetManager::createLabel("어빌리티 스톤 미착용");
        ui->vLayout2->addWidget(pLabel);
        return;
    }

    initializeLayout1(abilityStone.iconPath, abilityStone.itemGrade);
    initializeLayout2(abilityStone.name, abilityStone.itemGrade, abilityStone.engraves);
}

AbilityStoneInfo::~AbilityStoneInfo()
{
    delete ui;
}

void AbilityStoneInfo::initializeLayout1(const QString &iconPath, const ItemGrade &itemGrade)
{
    addAbilityStoneIcon(iconPath, itemGrade);
}

void AbilityStoneInfo::addAbilityStoneIcon(const QString &iconPath, ItemGrade itemGrade)
{
    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished,
            pNetworkManager, &QNetworkAccessManager::deleteLater);

    QLabel *pIcon = WidgetManager::createIcon(
        iconPath, pNetworkManager, itemGradeToBGColor(itemGrade));
    ui->vLayout1->addWidget(pIcon);
}

void AbilityStoneInfo::initializeLayout2(const QString &itemName, const ItemGrade &itemGrade, const QHash<QString, int> &engraves)
{
    addItemNameInfo(itemName, itemGrade);
    addEngraveInfo(engraves);
}

void AbilityStoneInfo::addItemNameInfo(const QString &itemName, ItemGrade itemGrade)
{
    QHBoxLayout *pHLayout = createHLayout(ui->vLayout2);

    QLabel *pLabelItemName = WidgetManager::createLabel(itemName, 10, 125);
    pLabelItemName->setStyleSheet(
        QString("QLabel { color: %1 }").arg(itemGradeToTextColor(itemGrade)));
    pHLayout->addWidget(pLabelItemName);
}

void AbilityStoneInfo::addEngraveInfo(const QHash<QString, int> &engraves)
{
    const QStringList &engraveNames = engraves.keys();
    const QString infoText = "%1 +%2";
    const QString infoStyle = "QLabel { color: %1 }";

    QString penalty;

    for (const QString &engraveName : engraveNames) {
        if (engraveName.contains("감소")) {
            penalty = engraveName;
        } else {
            QHBoxLayout *pHLayout = createHLayout(ui->vLayout2);
            QLabel *pLabelEngraveInfo = WidgetManager::createLabel(
                infoText.arg(engraveName).arg(engraves[engraveName]), 10, 100);

            pLabelEngraveInfo->setStyleSheet(infoStyle.arg("#FFA500"));
            pHLayout->addWidget(pLabelEngraveInfo);
        }
    }

    QHBoxLayout *pHLayout = createHLayout(ui->vLayout2);
    QLabel *pLabelPenalty = WidgetManager::createLabel(
        infoText.arg(penalty).arg(engraves[penalty]), 10, 100);
    pLabelPenalty->setStyleSheet(infoStyle.arg("red"));
    pHLayout->addWidget(pLabelPenalty);
}

QHBoxLayout *AbilityStoneInfo::createHLayout(QVBoxLayout *pLayout)
{
    QHBoxLayout *pHLayout = new QHBoxLayout();

    pHLayout->setSpacing(5);
    pLayout->addLayout(pHLayout);
    pLayout->setAlignment(pHLayout, Qt::AlignLeft);

    return pHLayout;
}
