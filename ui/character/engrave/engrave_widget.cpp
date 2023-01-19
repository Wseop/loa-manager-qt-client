#include "engrave_widget.h"
#include "ui_engrave_widget.h"
#include "game_data/character/engrave/engrave.h"
#include "game_data/character/engrave/engrave_manager.h"
#include "ui/widget_manager.h"
#include "ui/font_manager.h"
#include <QLabel>
#include <QVBoxLayout>

EngraveWidget::EngraveWidget(QWidget* pParent, const Engrave* pEngrave) :
    QWidget(pParent),
    ui(new Ui::EngraveWidget),
    m_pEngrave(pEngrave)
{
    ui->setupUi(this);
    ui->hLayoutGroupEngrave->setAlignment(Qt::AlignHCenter);

    setFonts();
    addEngraveLayouts();
}

EngraveWidget::~EngraveWidget()
{
    for (QLabel* pLabel : m_labels)
        delete pLabel;
    for (QVBoxLayout* pVLayout : m_vLayouts)
        delete pVLayout;
    delete ui;
}

void EngraveWidget::setFonts()
{
    FontManager* pFontManager = FontManager::getInstance();
    QFont nanumBold10 = pFontManager->getFont(FontFamily::NanumSquareNeoBold, 10);

    ui->groupEngrave->setFont(nanumBold10);
}

void EngraveWidget::addEngraveLayouts()
{
    EngraveManager* pEngraveManager = EngraveManager::getInstance();

    const QList<PairEngraveValue>& engraves = m_pEngrave->getEngraves();
    for (const PairEngraveValue& pairEngraveValue : engraves)
    {
        QVBoxLayout* pVLayout = new QVBoxLayout();
        pVLayout->setAlignment(Qt::AlignVCenter);
        m_vLayouts.append(pVLayout);
        ui->hLayoutGroupEngrave->addLayout(pVLayout);

        QLabel* pIcon = WidgetManager::createIcon(pEngraveManager->iconPath(pairEngraveValue.first), nullptr, this);
        m_labels.append(pIcon);
        pVLayout->addWidget(pIcon);

        QLabel* pLabelLevel = WidgetManager::createLabel("Lv. " + QString::number(pairEngraveValue.second / 5), LABEL_WIDTH, LABEL_HEIGHT, 10, this, "");
        m_labels.append(pLabelLevel);
        pVLayout->addWidget(pLabelLevel);
    }

    const QList<PairEngraveValue>& penalties = m_pEngrave->getPenalties();
    for (const PairEngraveValue& pairEngraveValue : penalties)
    {
        QVBoxLayout* pVLayout = new QVBoxLayout();
        pVLayout->setAlignment(Qt::AlignVCenter);
        m_vLayouts.append(pVLayout);
        ui->hLayoutGroupEngrave->addLayout(pVLayout);

        QLabel* pIcon = WidgetManager::createIcon(pEngraveManager->iconPath(pairEngraveValue.first), nullptr, this);
        m_labels.append(pIcon);
        pVLayout->addWidget(pIcon);

        QLabel* pLabelLevel = WidgetManager::createLabel("Lv. " + QString::number(pairEngraveValue.second / 5), LABEL_WIDTH, LABEL_HEIGHT, 10, this, "red");
        m_labels.append(pLabelLevel);
        pVLayout->addWidget(pLabelLevel);
    }
}
