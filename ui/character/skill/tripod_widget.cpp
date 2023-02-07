#include "tripod_widget.h"
#include "ui_tripod_widget.h"
#include "ui/widget_manager.h"
#include "ui/font_manager.h"
#include <QLabel>

TripodWidget::TripodWidget(QWidget* pParent, const QList<Tripod>& tripods) :
    QWidget(pParent),
    ui(new Ui::TripodWidget),
    m_tripods(tripods)
{
    ui->setupUi(this);

    addLabels();
    setFonts();
}

TripodWidget::~TripodWidget()
{
    for (QLabel* pLabel : m_labels)
        delete pLabel;
    delete ui;
}

void TripodWidget::addLabels()
{
    for (int i = 0; i < m_tripods.size(); i++)
    {
        const Tripod& tripod = m_tripods[i];
        QString labelText = QString("%1. %2 Lv.%3").arg(i + 1).arg(tripod.name).arg(tripod.level);
        QLabel* pLabelTripod = WidgetManager::createLabel(labelText, 10, tripodColorCode(i), LABEL_WIDTH, LABEL_HEIGHT);
        m_labels.append(pLabelTripod);
        ui->vLayoutGroupTripod->addWidget(pLabelTripod);
    }
}

void TripodWidget::setFonts()
{
    FontManager* pFontManager = FontManager::getInstance();
    QFont nanumBold10 = pFontManager->getFont(FontFamily::NanumSquareNeoBold, 10);

    ui->groupTripod->setFont(nanumBold10);
}
