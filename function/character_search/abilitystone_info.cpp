#include "abilitystone_info.h"
#include "ui_abilitystone_info.h"
#include "ui/widget_manager.h"
#include "game/item/abilitystone.h"

#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkReply>

AbilityStoneInfo::AbilityStoneInfo(const AbilityStone *pAbilityStone) :
    ui(new Ui::AbilityStoneInfo)
{
    ui->setupUi(this);

    if (pAbilityStone == nullptr)
    {
        QLabel *pLabel = WidgetManager::createLabel("어빌리티 스톤 미착용");
        ui->vLayoutRight->addWidget(pLabel);
        mWidgets << pLabel;
        return;
    }

    initializeIcon(pAbilityStone->iconPath(), pAbilityStone->itemGrade());
    initializeItemNameInfo(pAbilityStone->itemName(), pAbilityStone->itemGrade());
    initializeEngraveInfo(pAbilityStone->getEngraves(), pAbilityStone);
    initializePenaltyInfo(pAbilityStone->getPenalties().at(0), pAbilityStone->getPenaltyValue(pAbilityStone->getPenalties().at(0)));
}

AbilityStoneInfo::~AbilityStoneInfo()
{
    for (QWidget *pWidget : mWidgets)
        delete pWidget;
    mWidgets.clear();

    for (QLayout *pLayout : mLayouts)
        delete pLayout;
    mLayouts.clear();

    delete ui;
}

void AbilityStoneInfo::initializeIcon(const QString &iconPath, ItemGrade itemGrade)
{
    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    QLabel *pIcon = WidgetManager::createIcon(iconPath, pNetworkManager, itemGradeToBGColor(itemGrade));
    ui->vLayoutLeft->addWidget(pIcon);
    mWidgets << pIcon;
}

void AbilityStoneInfo::initializeItemNameInfo(const QString &itemName, ItemGrade itemGrade)
{
    QHBoxLayout *pHLayout = createHLayout();

    QLabel *pLabelItemName = WidgetManager::createLabel(itemName, 10, itemGradeToTextColor(itemGrade), 125);
    pHLayout->addWidget(pLabelItemName);
    mWidgets << pLabelItemName;
}

void AbilityStoneInfo::initializeEngraveInfo(const QStringList &engraves, const AbilityStone *pAbilityStone)
{
    QHBoxLayout *pHLayout = createHLayout();

    for (const QString &engrave : engraves)
    {
        QString text = QString("%1 +%2").arg(engrave).arg(pAbilityStone->getEngraveValue(engrave));

        QLabel *pLabelEngrave = WidgetManager::createLabel(text, 10, "#FFA500", 100);
        pHLayout->addWidget(pLabelEngrave);
        mWidgets << pLabelEngrave;
    }
}

void AbilityStoneInfo::initializePenaltyInfo(const QString &penalty, int value)
{
    QHBoxLayout *pHLayout = createHLayout();
    QString text = QString("%1 +%2").arg(penalty).arg(value);

    QLabel *pLabelPenalty = WidgetManager::createLabel(text, 10, "red", 100);
    pHLayout->addWidget(pLabelPenalty);
    mWidgets << pLabelPenalty;
}

QHBoxLayout *AbilityStoneInfo::createHLayout()
{
    QHBoxLayout *pHLayout = new QHBoxLayout();

    pHLayout->setSpacing(5);
    ui->vLayoutRight->addLayout(pHLayout);
    ui->vLayoutRight->setAlignment(pHLayout, Qt::AlignLeft);
    mLayouts << pHLayout;

    return pHLayout;
}
