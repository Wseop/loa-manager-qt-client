#include "collectible_widget.h"
#include "ui_collectible_widget.h"
#include "ui/widget_manager.h"
#include "game_data/character/collectible/collectible.h"
#include <QLabel>

CollectibleWidget::CollectibleWidget(QWidget* pParent, const QList<Collectible*>& collectibles) :
    QWidget(pParent),
    ui(new Ui::CollectibleWidget),
    m_collectibles(collectibles)
{
    ui->setupUi(this);
    ui->vLayoutCollectible->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    addCollectibleLabels();
}

CollectibleWidget::~CollectibleWidget()
{
    delete ui;
}

void CollectibleWidget::addCollectibleLabels()
{
    for (const Collectible* pCollectible : m_collectibles)
    {
        QString labelText;
        labelText += collectibleToStr(pCollectible->getType());
        labelText += QString(" (%1 / %2)").arg(pCollectible->getPoint(), pCollectible->getMaxPoint());

        QLabel* pCollectibleLabel = WidgetManager::createLabel(labelText, LABEL_WIDTH, LABEL_HEIGHT, 10, this);
        m_labels.append(pCollectibleLabel);
        ui->vLayoutCollectible->addWidget(pCollectibleLabel);
    }
}
