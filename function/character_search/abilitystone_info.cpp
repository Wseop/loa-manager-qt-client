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
        ui->vLayout2->addWidget(pLabel);
        mWidgets << pLabel;
        return;
    }

    initializeLayout1(pAbilityStone);
    initializeLayout2(pAbilityStone);
}

AbilityStoneInfo::~AbilityStoneInfo()
{
    for (QWidget *pWidget : mWidgets)
        delete pWidget;
    mWidgets.clear();

    for (auto rIter = mLayouts.rbegin(); rIter != mLayouts.rend(); rIter++)
        delete *rIter;
    mLayouts.clear();

    delete ui;
}

void AbilityStoneInfo::initializeLayout1(const AbilityStone *pAbilityStone)
{
    addAbilityStoneIcon(pAbilityStone->iconPath(), pAbilityStone->itemGrade());
}

void AbilityStoneInfo::addAbilityStoneIcon(const QString &iconPath, ItemGrade itemGrade)
{
    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    QLabel *pIcon = WidgetManager::createIcon(iconPath, pNetworkManager, itemGradeToBGColor(itemGrade));
    ui->vLayout1->addWidget(pIcon);
    mWidgets << pIcon;
}

void AbilityStoneInfo::initializeLayout2(const AbilityStone *pAbilityStone)
{
    addItemNameInfo(pAbilityStone->itemName(), pAbilityStone->itemGrade());
    addEngraveInfo(pAbilityStone->getEngraves(), pAbilityStone->getPenalties(), pAbilityStone);
}

void AbilityStoneInfo::addItemNameInfo(const QString &itemName, ItemGrade itemGrade)
{
    QHBoxLayout *pHLayout = createHLayout(ui->vLayout2);

    QLabel *pLabelItemName = WidgetManager::createLabel(itemName, 10, itemGradeToTextColor(itemGrade), 125);
    pHLayout->addWidget(pLabelItemName);
    mWidgets << pLabelItemName;
}

void AbilityStoneInfo::addEngraveInfo(const QStringList &engraves, const QStringList &penalties, const AbilityStone *pAbilityStone)
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
                text = text.arg(engrave).arg(pAbilityStone->getEngraveValue(engrave));
            else if (i == 1)
                text = text.arg(engrave).arg(pAbilityStone->getPenaltyValue(engrave));

            QLabel *pLabelEngrave = WidgetManager::createLabel(text, 10, textColor[i], 100);
            pHLayout->addWidget(pLabelEngrave);
            mWidgets << pLabelEngrave;
        }
    }
}

QHBoxLayout *AbilityStoneInfo::createHLayout(QVBoxLayout *pLayout)
{
    QHBoxLayout *pHLayout = new QHBoxLayout();

    pHLayout->setSpacing(5);
    pLayout->addLayout(pHLayout);
    pLayout->setAlignment(pHLayout, Qt::AlignLeft);
    mLayouts << pHLayout;

    return pHLayout;
}