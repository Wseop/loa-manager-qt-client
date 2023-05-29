#include "equip_info.h"
#include "ui_equip_info.h"
#include "ui/widget_manager.h"
#include "resource/resource_manager.h"

#include <QLabel>
#include <QProgressBar>
#include <QNetworkAccessManager>
#include <QNetworkReply>

EquipInfo::EquipInfo(const Equipment &equipment) :
    ui(new Ui::EquipInfo)
{
    ui->setupUi(this);

    if (equipment.type == "")
    {
        QLabel *pLabel = WidgetManager::createLabel("장비 미착용");
        ui->vLayout2->addWidget(pLabel);
        return;
    }

    initializeLayout1(equipment.iconPath, equipment.itemGrade, equipment.quality);
    initializeLayout2(equipment);
}

EquipInfo::~EquipInfo()
{
    delete ui;
}

void EquipInfo::initializeLayout1(const QString &iconPath, const ItemGrade &itemGrade, int quality)
{
    addEquipIcon(iconPath, itemGrade);
    addQualityBar(quality);
}

void EquipInfo::addEquipIcon(const QString &iconPath, ItemGrade itemGrade)
{
    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished,
            pNetworkManager, &QNetworkAccessManager::deleteLater);

    QLabel *pIcon = WidgetManager::createIcon(
        iconPath, pNetworkManager, itemGradeToBGColor(itemGrade));
    ui->vLayout1->addWidget(pIcon);
}

void EquipInfo::addQualityBar(int quality)
{
    QProgressBar *pQualityBar = WidgetManager::createQualityBar(quality, 50, 25);
    ui->vLayout1->addWidget(pQualityBar);
}

void EquipInfo::initializeLayout2(const Equipment &equipment)
{
    addItemSourceInfo(equipment.itemSet.first, equipment.name, equipment.itemGrade);
    addReforgeLevelInfo(equipment.name, equipment.itemLevel);
    addItemSetInfo(equipment.itemSet);

    if (equipment.type == "무기") {
        addEllaInfo(equipment.isElla);
    } else {
        addElixirInfo(equipment.elixirs);
    }
}

void EquipInfo::addItemSourceInfo(const QString &setName, const QString &itemName, ItemGrade itemGrade)
{
    QString sourceInfo;

    // 군단장 제작 장비가 아니면 아이템 등급으로 표시
    if (setName == "" || itemGrade == ItemGrade::에스더)
        sourceInfo = itemGradeToStr(itemGrade);
    else
    {
        // 군단장 제작 장비의 경우 해당하는 군단장명으로 표시
        const QStringList &setNames = ResourceManager::getInstance()->equipSetNames(
            setName);

        for (int i = 0; i < setNames.size(); i++)
        {
            if (itemName.contains(setNames[i]))
            {
                if (i < 2)
                    sourceInfo = "일리아칸";
                else if (i < 4)
                    sourceInfo = "하브";
                else if (i == 4)
                    sourceInfo = "노브";
                else if (i == 5)
                    sourceInfo = "발비";
                break;
            }
        }
    }

    QLabel *pLabelItemSource = WidgetManager::createLabel(sourceInfo, 10, 100);
    pLabelItemSource->setStyleSheet(QString("QLabel { border: 1px solid black; "
                                            "         border-radius: 5px;"
                                            "         padding: 2px;"
                                            "         color: %1 }").arg(
                                            itemGradeToTextColor(itemGrade)));
    ui->vLayout2->addWidget(pLabelItemSource);
    ui->vLayout2->setAlignment(pLabelItemSource, Qt::AlignLeft);
}

void EquipInfo::addReforgeLevelInfo(const QString &itemName, int itemLevel)
{
    int reforge = 0;

    if (itemName.startsWith("+")) {
        reforge = itemName.sliced(1, itemName.indexOf(' ') - 1).toInt();
    }

    const QString text = QString("+%1 (%2)").arg(reforge).arg(itemLevel);

    QLabel *pLabelReforgeLevel = WidgetManager::createLabel(text, 10, 100);
    ui->vLayout2->addWidget(pLabelReforgeLevel);
    ui->vLayout2->setAlignment(pLabelReforgeLevel, Qt::AlignLeft);
}

void EquipInfo::addItemSetInfo(const QPair<QString, int> &itemSet)
{
    QLabel *pLabelItemSet = WidgetManager::createLabel(
        QString("%1 Lv.%2").arg(itemSet.first).arg(itemSet.second), 10, 100);

    ui->vLayout2->addWidget(pLabelItemSet);
    ui->vLayout2->setAlignment(pLabelItemSet, Qt::AlignLeft);
}

void EquipInfo::addElixirInfo(const QHash<QString, int> &elixirs)
{
    QHBoxLayout *pHLayout = new QHBoxLayout();
    pHLayout->setSpacing(5);
    ui->vLayout2->addLayout(pHLayout);

    for (auto iter = elixirs.constBegin(); iter != elixirs.constEnd(); iter++) {
        QLabel *pLabelElixir = WidgetManager::createLabel(
            QString("%1 Lv.%2").arg(iter.key()).arg(iter.value()), 10, 130);

        pLabelElixir->setStyleSheet("QLabel { border: 1px solid black; "
                                    "         border-radius: 5px; "
                                    "         padding: 2px }");
        pHLayout->addWidget(pLabelElixir);
    }
}

void EquipInfo::addEllaInfo(bool bElla)
{
    if (bElla)
    {
        QLabel *pLabelElla = WidgetManager::createLabel("엘라 부여", 10, 100);
        pLabelElla->setStyleSheet(QString("QLabel { border: 1px solid black; "
                                          "         border-radius: 5px; "
                                          "         padding: 2px;"
                                          "         color: %1 }").arg(itemGradeToTextColor(ItemGrade::에스더)));
        ui->vLayout2->addWidget(pLabelElla);
    }
}
