#include "accessory_widget.h"
#include "ui_accessory_widget.h"
#include "game_data/character/item/accessory.h"
#include "ui/widget_manager.h"
#include <QLabel>
#include <QProgressBar>
#include <QNetworkAccessManager>

AccessoryWidget::AccessoryWidget(QWidget* pParent, const Accessory* pAccessory) :
    QWidget(pParent),
    ui(new Ui::AccessoryWidget),
    m_pAccessory(pAccessory),
    m_pQualityBar(nullptr),
    m_pNetworkManager(new QNetworkAccessManager())
{
    ui->setupUi(this);

    addIcon();
    addQualityBar();
    addLabels();
}

AccessoryWidget::~AccessoryWidget()
{
    for (QLabel* pLabel : m_labels)
        delete pLabel;
    delete m_pQualityBar;
    delete m_pNetworkManager;
    delete ui;
}

void AccessoryWidget::addIcon()
{
    QLabel* pIcon = WidgetManager::createIcon(m_pAccessory->getIconPath(), m_pNetworkManager, this);
    m_labels.append(pIcon);
    ui->vLayoutLeft->addWidget(pIcon);
}

void AccessoryWidget::addQualityBar()
{
    m_pQualityBar = WidgetManager::createQualityBar(m_pAccessory->getQuality(), QUALITYBAR_WIDTH, QUALITYBAR_HEIGHT, 10, this);
    ui->vLayoutLeft->addWidget(m_pQualityBar);
}

void AccessoryWidget::addLabels()
{
    // 악세서리 이름 추가
    QLabel* pLabelName = WidgetManager::createLabel(m_pAccessory->getName(), LABEL_WIDTH, LABEL_HEIGHT, 10, this);
    m_labels.append(pLabelName);
    ui->vLayoutRight->addWidget(pLabelName);

    // 특성 추가
    QString abilityText;
    const QMap<Ability, int>& abilities = m_pAccessory->getAbilities();
    for (auto iter = abilities.begin(); iter != abilities.end(); iter++)
    {
        abilityText += QString("%1 +%2  ").arg(abilityToStr(iter.key()).arg(iter.value()));
    }
    QLabel* pLabelAbility = WidgetManager::createLabel(abilityText, LABEL_WIDTH, LABEL_HEIGHT, 10, this);
    m_labels.append(pLabelAbility);
    ui->vLayoutRight->addWidget(pLabelAbility);

    // 각인 추가
    const QMap<QString, int>& engraves = m_pAccessory->getEngraves();
    for (auto iter = engraves.begin(); iter != engraves.end(); iter++)
    {
        QString engraveText = QString("[%1] +%2").arg(iter.key()).arg(iter.value());
        QLabel* pEngraveLabel = WidgetManager::createLabel(engraveText, LABEL_WIDTH, LABEL_HEIGHT, 10, this);
        m_labels.append(pEngraveLabel);
        ui->vLayoutRight->addWidget(pEngraveLabel);
    }
    const QMap<QString, int>& penalties = m_pAccessory->getPenalties();
    for (auto iter = penalties.begin(); iter != penalties.end(); iter++)
    {
        QString penaltyText = QString("[%1] +%2").arg(iter.key()).arg(iter.value());
        QLabel* pPenaltyLabel = WidgetManager::createLabel(penaltyText, LABEL_WIDTH, LABEL_HEIGHT, 10, this);
        m_labels.append(pPenaltyLabel);
        ui->vLayoutRight->addWidget(pPenaltyLabel);
    }
}
