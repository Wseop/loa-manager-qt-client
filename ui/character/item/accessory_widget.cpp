#include "accessory_widget.h"
#include "ui_accessory_widget.h"
#include "game_data/character/item/accessory.h"
#include "ui/widget_manager.h"
#include "ui/font_manager.h"
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
    ui->groupAccessory->setTitle(itemTypeToStr(m_pAccessory->getType()));
    ui->vLayoutLeft->setAlignment(Qt::AlignVCenter);

    addIcon();
    addQualityBar();
    addLabels();
    setFonts();
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
    QLabel* pIcon = WidgetManager::createIcon(m_pAccessory->getIconPath(), m_pNetworkManager, ICON_WIDTH, ICON_HEIGHT, this);
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
    // 특성 추가
    QString abilityText;
    const QMap<Ability, int>& abilities = m_pAccessory->getAbilities();
    for (auto iter = abilities.begin(); iter != abilities.end(); iter++)
    {
        abilityText += QString("%1 +%2,  ").arg(abilityToStr(iter.key())).arg(iter.value());
    }
    abilityText.chop(3);
    QLabel* pLabelAbility = WidgetManager::createLabel(abilityText, LABEL_WIDTH, LABEL_HEIGHT, 10, this, colorCode(m_pAccessory->getGrade()));
    m_labels.append(pLabelAbility);
    ui->vLayoutRight->addWidget(pLabelAbility);

    // 각인 추가
    const QList<PairEngraveValue>& engraves = m_pAccessory->getEngraves();
    for (const PairEngraveValue& pairEngraveValue : engraves)
    {
        QString engraveText = QString("[%1] +%2").arg(pairEngraveValue.first).arg(pairEngraveValue.second);
        QLabel* pEngraveLabel = WidgetManager::createLabel(engraveText, LABEL_WIDTH, LABEL_HEIGHT, 10, this);
        m_labels.append(pEngraveLabel);
        ui->vLayoutRight->addWidget(pEngraveLabel);
    }
    const QList<PairEngraveValue>& penalties = m_pAccessory->getPenalties();
    for (const PairEngraveValue& pairEngraveValue : penalties)
    {
        QString penaltyText = QString("[%1] +%2").arg(pairEngraveValue.first).arg(pairEngraveValue.second);
        QLabel* pPenaltyLabel = WidgetManager::createLabel(penaltyText, LABEL_WIDTH, LABEL_HEIGHT, 10, this, "red");
        m_labels.append(pPenaltyLabel);
        ui->vLayoutRight->addWidget(pPenaltyLabel);
    }
}

void AccessoryWidget::setFonts()
{
    FontManager* pFontManager = FontManager::getInstance();
    QFont nanumBold10 = pFontManager->getFont(FontFamily::NanumSquareNeoBold, 10);

    ui->groupAccessory->setFont(nanumBold10);
}
