#include "gem_widget.h"
#include "ui_gem_widget.h"
#include "game_data/character/item/gem.h"
#include "ui/widget_manager.h"
#include <QLabel>
#include <QNetworkAccessManager>

GemWidget::GemWidget(QWidget* pParent, const Gem* pGem) :
    QWidget(pParent),
    ui(new Ui::GemWidget),
    m_pGem(pGem),
    m_pNetworkManager(new QNetworkAccessManager())
{
    ui->setupUi(this);

    addIcon();
    addLabels();
}

GemWidget::~GemWidget()
{
    for (QLabel* pLabel : m_labels)
        delete pLabel;
    delete m_pNetworkManager;
    delete ui;
}

void GemWidget::addIcon()
{
    QLabel* pIcon = WidgetManager::createIcon(m_pGem->getIconPath(), m_pNetworkManager, this);
    m_labels.append(pIcon);
    ui->vLayoutIcon->addWidget(pIcon);
}

void GemWidget::addLabels()
{
    QLabel* pLabelName = WidgetManager::createLabel(m_pGem->getName(), LABEL_WIDTH, LABEL_HEIGHT, 10, this);
    m_labels.append(pLabelName);
    ui->vLayoutInfo->addWidget(pLabelName);

    QLabel* pLabelEffect = WidgetManager::createLabel(m_pGem->getEffect(), LABEL_WIDTH, LABEL_HEIGHT, 10, this);
    m_labels.append(pLabelEffect);
    ui->vLayoutInfo->addWidget(pLabelEffect);
}
