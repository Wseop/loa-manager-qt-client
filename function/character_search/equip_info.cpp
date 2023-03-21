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
        ui->vLayoutRight->addWidget(pLabel);
        mWidgets << pLabel;
        return;
    }

    QString iconPath = pEquip->iconPath();
    ItemGrade itemGrade = pEquip->itemGrade();
    int quality = 0;
    int reforge = 0;
    int itemLevel = 0;
    ItemSet itemSet = ItemSet::size;
    int setLevel = 0;
    bool bElla = false;
    QList<Elixir> elixirs;
    ArmorPart part = ArmorPart::size;

    if (pEquip->itemType() == ItemType::Weapon)
    {
        const Weapon *pWeapon = static_cast<const Weapon*>(pEquip);

        quality = pWeapon->quality();
        reforge = pWeapon->reforge();
        itemLevel = pWeapon->itemLevel();
        itemSet = pWeapon->itemSet();
        setLevel = pWeapon->setLevel();
        bElla = pWeapon->ella();
    }
    else if (pEquip->itemType() == ItemType::Armor)
    {
        const Armor *pArmor = static_cast<const Armor*>(pEquip);

        quality = pArmor->quality();
        reforge = pArmor->reforge();
        itemLevel = pArmor->itemLevel();
        itemSet = pArmor->itemSet();
        setLevel = pArmor->setLevel();
        elixirs = pArmor->elixirs();
        part = pArmor->armorPart();
    }
    else
        return;

    initializeIcon(iconPath, itemGrade);
    initializeQualityBar(quality);
    initializeReforgeLevel(reforge, itemLevel, itemGrade);
    initializeItemSet(itemSet, setLevel);

    if (pEquip->itemType() == ItemType::Weapon)
        initializeElla(bElla);
    else if (pEquip->itemType() == ItemType::Armor)
        initializeElixir(elixirs, part);
}

EquipInfo::~EquipInfo()
{
    for (QWidget *pWidget : mWidgets)
        delete pWidget;
    mWidgets.clear();

    for (QLayout *pLayout : mLayouts)
        delete pLayout;
    mLayouts.clear();

    delete ui;
}

void EquipInfo::initializeIcon(const QString &iconPath, ItemGrade itemGrade)
{
    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    QLabel *pIcon = WidgetManager::createIcon(iconPath, pNetworkManager, itemGradeToBGColor(itemGrade));
    ui->vLayoutLeft->addWidget(pIcon);
    mWidgets << pIcon;
}

void EquipInfo::initializeQualityBar(int quality)
{
    QProgressBar *pQualityBar = WidgetManager::createQualityBar(quality, 50, 25);
    ui->vLayoutLeft->addWidget(pQualityBar);
    mWidgets << pQualityBar;
}

void EquipInfo::initializeReforgeLevel(int reforge, int itemLevel, ItemGrade itemGrade)
{
    QString text = QString("+%1 (%2)").arg(reforge).arg(itemLevel);
    QLabel *pLabelReforgeLevel = WidgetManager::createLabel(text, 10, itemGradeToTextColor(itemGrade), 100);
    ui->vLayoutRight->addWidget(pLabelReforgeLevel);
    ui->vLayoutRight->setAlignment(pLabelReforgeLevel, Qt::AlignLeft);
    mWidgets << pLabelReforgeLevel;
}

void EquipInfo::initializeItemSet(ItemSet itemSet, int setLevel)
{
    QString text = QString("%1 Lv.%2").arg(itemSetToQString(itemSet)).arg(setLevel);
    QLabel *pLabelItemSet = WidgetManager::createLabel(text, 10, "", 100);
    ui->vLayoutRight->addWidget(pLabelItemSet);
    ui->vLayoutRight->setAlignment(pLabelItemSet, Qt::AlignLeft);
    mWidgets << pLabelItemSet;
}

void EquipInfo::initializeElixir(const QList<Elixir> &elixirs, ArmorPart part)
{
    QHBoxLayout *pHLayout = new QHBoxLayout();
    pHLayout->setSpacing(5);
    ui->vLayoutRight->addLayout(pHLayout);
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

void EquipInfo::initializeElla(bool bElla)
{
    if (bElla)
    {
        QLabel *pLabelElla = WidgetManager::createLabel("엘라 부여", 10, "", 100);
        pLabelElla->setStyleSheet(QString("QLabel { border: 1px solid black; "
                                          "         border-radius: 5px; padding: 2px;"
                                          "         color: %1 }").arg(itemGradeToTextColor(ItemGrade::에스더)));
        ui->vLayoutRight->addWidget(pLabelElla);
        mWidgets << pLabelElla;
    }
}
