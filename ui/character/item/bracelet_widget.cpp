#include "bracelet_widget.h"
#include "ui_bracelet_widget.h"
#include "game_data/character/item/bracelet.h"
#include "ui/widget_manager.h"
#include <QLabel>
#include <QNetworkAccessManager>

BraceletWidget::BraceletWidget(QWidget* pParent, const Bracelet* pBracelet) :
    QWidget(pParent),
    ui(new Ui::BraceletWidget),
    m_pBracelet(pBracelet),
    m_pNetworkManager(new QNetworkAccessManager)
{
    ui->setupUi(this);

    addIcon();
    addLabels();
}

BraceletWidget::~BraceletWidget()
{
    for (QLabel* pLabel : m_labels)
        delete pLabel;
    delete m_pNetworkManager;
    delete ui;
}

void BraceletWidget::addIcon()
{
    QLabel* pIcon = WidgetManager::createIcon(m_pBracelet->getIconPath(), m_pNetworkManager, this);
    m_labels.append(pIcon);
    ui->vLayoutIcon->addWidget(pIcon);
}

void BraceletWidget::addLabels()
{
    QLabel* pLabelName = WidgetManager::createLabel(m_pBracelet->getName(), LABEL_WIDTH, LABEL_HEIGHT, 10, this);
    m_labels.append(pLabelName);
    ui->vLayoutInfo->addWidget(pLabelName);

    const QStringList& effects = m_pBracelet->getEffects();
    for (const QString& effect : effects)
    {
        QLabel* pLabelEffect = WidgetManager::createLabel(effect, LABEL_WIDTH, LABEL_HEIGHT, 10, this);
        m_labels.append(pLabelEffect);
        ui->vLayoutInfo->addWidget(pLabelEffect);
    }
    const QStringList& specialEffects = m_pBracelet->getSpecialEffects();
    for (const QString& effect : specialEffects)
    {
        QLabel* pLabelEffect = WidgetManager::createLabel(effect, LABEL_WIDTH, LABEL_HEIGHT, 10, this);
        m_labels.append(pLabelEffect);
        ui->vLayoutInfo->addWidget(pLabelEffect);
    }
}
