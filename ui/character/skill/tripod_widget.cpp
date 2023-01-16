#include "tripod_widget.h"
#include "ui_tripod_widget.h"
#include "ui/widget_manager.h"
#include <QLabel>

TripodWidget::TripodWidget(QWidget* pParent, const QList<Tripod>& tripods) :
    QWidget(pParent),
    ui(new Ui::TripodWidget),
    m_tripods(tripods)
{
    ui->setupUi(this);

    addLabels();
}

TripodWidget::~TripodWidget()
{
    delete ui;
}

void TripodWidget::addLabels()
{
    for (int i = 0; i < m_tripods.size(); i++)
    {
        const Tripod& tripod = m_tripods[i];
        QString labelText = QString("%1 Lv.%2").arg(tripod.name).arg(tripod.level);
        QLabel* pLabelTripod = WidgetManager::createLabel(labelText, LABEL_WIDTH, LABEL_HEIGHT, 10, this);
        m_labels.append(pLabelTripod);
        ui->vLayoutTripod->addWidget(pLabelTripod);
    }
}
