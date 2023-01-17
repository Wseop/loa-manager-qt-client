#include "equip_widget.h"
#include "ui_equip_widget.h"
#include "ui/widget_manager.h"
#include "game_data/character/item/equip.h"
#include <QLabel>
#include <QProgressBar>
#include <QNetworkAccessManager>

EquipWidget::EquipWidget(QWidget* pParent, const Equip* pEquip) :
    QWidget(pParent),
    ui(new Ui::EquipWidget),
    m_pEquip(pEquip),
    m_pQualityBar(nullptr),
    m_pNetworkManager(new QNetworkAccessManager())
{
    ui->setupUi(this);
    ui->groupEquip->setTitle(itemTypeToStr(m_pEquip->getType()));

    addIcon();
    addQualityBar();
    addLabels();
}

EquipWidget::~EquipWidget()
{
    for (QLabel* pLabel : m_labels)
        delete pLabel;
    delete m_pQualityBar;
    delete m_pNetworkManager;
    delete ui;
}

void EquipWidget::addIcon()
{
    QLabel* pIcon = WidgetManager::createIcon(m_pEquip->getIconPath(), m_pNetworkManager, this);
    m_labels.append(pIcon);
    ui->vLayoutLeft->addWidget(pIcon);
}

void EquipWidget::addQualityBar()
{
    m_pQualityBar = WidgetManager::createQualityBar(m_pEquip->getQuality(), QUALITYBAR_WIDTH, QUALITYBAR_HEIGHT, 10, nullptr);
    ui->vLayoutLeft->addWidget(m_pQualityBar);
}

void EquipWidget::addLabels()
{
    // 강화수치 + 아이템 레벨 정보 추가
    QString equipTitle = "%1 (%2)";
    if (m_pEquip->getName().startsWith("+"))
    {
        QString reforge = m_pEquip->getName().sliced(0, m_pEquip->getName().indexOf(" "));
        const QString& levelTier = m_pEquip->getLevelTier();
        int levelIndex = levelTier.indexOf("레벨") + 3;
        QString level = levelTier.sliced(levelIndex, levelTier.indexOf("(") - levelIndex - 1);
        equipTitle = equipTitle.arg(reforge, level);
    }
    else
    {
        equipTitle = "";
    }
    QLabel* pEquipTitleLabel = WidgetManager::createLabel(equipTitle, LABEL_WIDTH, LABEL_HEIGHT, 10, this);
    m_labels.append(pEquipTitleLabel);
    ui->vLayoutRight->addWidget(pEquipTitleLabel);

    // 세트효과 or 엘라부여 정보 추가
    QString setEffect;
    if (m_pEquip->isElla())
        setEffect += "엘라 부여";
    else
    {
        setEffect += setEffectToStr(m_pEquip->getSetEffect());
        setEffect += QString(" Lv. %1").arg(m_pEquip->getSetLevel());
    }
    QLabel* pSetEffectLabel = WidgetManager::createLabel(setEffect, LABEL_WIDTH, LABEL_HEIGHT, 10, this);
    m_labels.append(pSetEffectLabel);
    ui->vLayoutRight->addWidget(pSetEffectLabel);
}
