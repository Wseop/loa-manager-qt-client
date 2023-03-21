#include "equip_info.h"
#include "ui_equip_info.h"
#include "ui/widget_manager.h"
#include "game/item/weapon.h"

#include <QLabel>
#include <QProgressBar>
#include <QNetworkAccessManager>
#include <QNetworkReply>

EquipInfo::EquipInfo(const Item *pEquip) :
    ui(new Ui::EquipInfo)
{
    ui->setupUi(this);

    if (pEquip == nullptr)
    {
        QLabel *pLabel = WidgetManager::createLabel("장비 미착용");
        ui->vLayout2->addWidget(pLabel);
        mWidgets << pLabel;
        return;
    }

    initializeLayout1(pEquip, pEquip->itemType());
    initializeLayout2(pEquip, pEquip->itemType());
}

EquipInfo::~EquipInfo()
{
    for (QWidget *pWidget : mWidgets)
        delete pWidget;
    mWidgets.clear();

    for (auto rIter = mLayouts.rbegin(); rIter != mLayouts.rend(); rIter++)
        delete *rIter;
    mLayouts.clear();

    delete ui;
}

void EquipInfo::initializeLayout1(const Item *pEquip, ItemType itemType)
{
    QString iconPath;
    ItemGrade itemGrade = ItemGrade::size;
    int quality = 0;

    if (itemType == ItemType::Weapon)
    {
        const Weapon *pWeapon = static_cast<const Weapon*>(pEquip);

        iconPath = pWeapon->iconPath();
        itemGrade = pWeapon->itemGrade();
        quality = pWeapon->quality();
    }
    else if (itemType == ItemType::Armor)
    {
        const Armor *pArmor = static_cast<const Armor*>(pEquip);

        iconPath = pArmor->iconPath();
        itemGrade = pArmor->itemGrade();
        quality = pArmor->quality();
    }

    addEquipIcon(iconPath, itemGrade);
    addQualityBar(quality);
}

void EquipInfo::addEquipIcon(const QString &iconPath, ItemGrade itemGrade)
{
    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    QLabel *pIcon = WidgetManager::createIcon(iconPath, pNetworkManager, itemGradeToBGColor(itemGrade));
    ui->vLayout1->addWidget(pIcon);
    mWidgets << pIcon;
}

void EquipInfo::addQualityBar(int quality)
{
    QProgressBar *pQualityBar = WidgetManager::createQualityBar(quality, 50, 25);
    ui->vLayout1->addWidget(pQualityBar);
    mWidgets << pQualityBar;
}

void EquipInfo::initializeLayout2(const Item *pEquip, ItemType itemType)
{
    int reforge = 0;
    int itemLevel = 0;
    ItemGrade itemGrade = ItemGrade::size;
    ItemSet itemSet = ItemSet::size;
    int setLevel = 0;
    QList<Elixir> elixirs;
    ArmorPart part = ArmorPart::size;
    bool bElla = false;

    if (itemType == ItemType::Weapon)
    {
        const Weapon *pWeapon = static_cast<const Weapon*>(pEquip);

        reforge = pWeapon->reforge();
        itemLevel = pWeapon->itemLevel();
        itemGrade = pWeapon->itemGrade();
        itemSet = pWeapon->itemSet();
        setLevel = pWeapon->setLevel();
        bElla = pWeapon->ella();
    }
    else if (itemType == ItemType::Armor)
    {
        const Armor *pArmor = static_cast<const Armor*>(pEquip);

        reforge = pArmor->reforge();
        itemLevel = pArmor->itemLevel();
        itemGrade = pArmor->itemGrade();
        itemSet = pArmor->itemSet();
        setLevel = pArmor->setLevel();
        elixirs = pArmor->elixirs();
        part = pArmor->armorPart();
    }

    addReforgeLevelInfo(reforge, itemLevel, itemGrade);
    addItemSetInfo(itemSet, setLevel);

    if (itemType == ItemType::Weapon)
        addEllaInfo(bElla);
    else if (itemType == ItemType::Armor)
        addElixirInfo(elixirs, part);
}

void EquipInfo::addReforgeLevelInfo(int reforge, int itemLevel, ItemGrade itemGrade)
{
    QString text = QString("+%1 (%2)").arg(reforge).arg(itemLevel);
    QLabel *pLabelReforgeLevel = WidgetManager::createLabel(text, 10, itemGradeToTextColor(itemGrade), 100);
    ui->vLayout2->addWidget(pLabelReforgeLevel);
    ui->vLayout2->setAlignment(pLabelReforgeLevel, Qt::AlignLeft);
    mWidgets << pLabelReforgeLevel;
}

void EquipInfo::addItemSetInfo(ItemSet itemSet, int setLevel)
{
    QString text = QString("%1 Lv.%2").arg(itemSetToQString(itemSet)).arg(setLevel);
    QLabel *pLabelItemSet = WidgetManager::createLabel(text, 10, "", 100);
    ui->vLayout2->addWidget(pLabelItemSet);
    ui->vLayout2->setAlignment(pLabelItemSet, Qt::AlignLeft);
    mWidgets << pLabelItemSet;
}

void EquipInfo::addElixirInfo(const QList<Elixir> &elixirs, ArmorPart part)
{
    QHBoxLayout *pHLayout = new QHBoxLayout();
    pHLayout->setSpacing(5);
    ui->vLayout2->addLayout(pHLayout);
    mLayouts << pHLayout;

    for (const Elixir &elixir : elixirs)
    {
        // 활성화가 가능한 엘릭서 정보만 추가
        if (elixir.part == ArmorPart::size || part == elixir.part)
        {
            QString text = QString("%1 Lv.%2").arg(elixir.effect).arg(elixir.level);
            QLabel *pLabelElixir = WidgetManager::createLabel(text, 10, "", 130);
            pLabelElixir->setStyleSheet("QLabel { border: 1px solid black; border-radius: 5px; padding: 2px }");
            pHLayout->addWidget(pLabelElixir);
            mWidgets << pLabelElixir;
        }
    }
}

void EquipInfo::addEllaInfo(bool bElla)
{
    if (bElla)
    {
        QLabel *pLabelElla = WidgetManager::createLabel("엘라 부여", 10, "", 100);
        pLabelElla->setStyleSheet(QString("QLabel { border: 1px solid black; "
                                          "         border-radius: 5px; padding: 2px;"
                                          "         color: %1 }").arg(itemGradeToTextColor(ItemGrade::에스더)));
        ui->vLayout2->addWidget(pLabelElla);
        mWidgets << pLabelElla;
    }
}
