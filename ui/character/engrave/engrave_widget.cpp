#include "engrave_widget.h"
#include "ui_engrave_widget.h"
#include "game_data/character/engrave/engrave.h"
#include "game_data/character/engrave/engrave_manager.h"
#include "ui/widget_manager.h"
#include <QLabel>
#include <QHBoxLayout>

EngraveWidget::EngraveWidget(QWidget* pParent, const Engrave* pEngrave) :
    QWidget(pParent),
    ui(new Ui::EngraveWidget),
    m_pEngrave(pEngrave)
{
    ui->setupUi(this);
    ui->vLayoutEngrave->setAlignment(Qt::AlignTop);

    addEngraveHLayouts();
}

EngraveWidget::~EngraveWidget()
{
    for (QLabel* pLabel : m_labels)
        delete pLabel;
    for (QHBoxLayout* pHLayout : m_hLayouts)
        delete pHLayout;
    delete ui;
}

void EngraveWidget::addEngraveHLayouts()
{
    const QMap<QString, int>& engraves = m_pEngrave->getEngraves();
    const QMap<QString, int>& penalties = m_pEngrave->getPenalties();

    for (auto iter = engraves.begin(); iter != engraves.end(); iter++)
    {
        QLabel* pIconLabel = WidgetManager::createIcon(EngraveManager::getInstance()->iconPath(iter.key()), nullptr, this);
        QLabel* pEngraveLabel = WidgetManager::createLabel(iter.key(), LABEL_ENGRAVE_WIDTH, LABEL_ENGRAVE_HEIGHT, 10, this);
        QLabel* pLevelLabel = WidgetManager::createLabel(QString("Lv. %1").arg(iter.value() / 5), LABEL_LEVEL_WIDTH, LABEL_LEVEL_HEIGHT, 10, this);

        m_labels.append(pIconLabel);
        m_labels.append(pEngraveLabel);
        m_labels.append(pLevelLabel);

        QHBoxLayout* pHLayout = new QHBoxLayout(this);
        pHLayout->addWidget(pIconLabel);
        pHLayout->addWidget(pEngraveLabel);
        pHLayout->addWidget(pLevelLabel);

        ui->vLayoutEngrave->addLayout(pHLayout);
    }
    for (auto iter = penalties.begin(); iter != penalties.end(); iter++)
    {
        QLabel* pIconLabel = WidgetManager::createIcon(EngraveManager::getInstance()->iconPath(iter.key()), nullptr, this);
        QLabel* pEngraveLabel = WidgetManager::createLabel(iter.key(), LABEL_ENGRAVE_WIDTH, LABEL_ENGRAVE_HEIGHT, 10, this);
        QLabel* pLevelLabel = WidgetManager::createLabel(QString("Lv. %1").arg(iter.value() / 5), LABEL_LEVEL_WIDTH, LABEL_LEVEL_HEIGHT, 10, this);

        m_labels.append(pIconLabel);
        m_labels.append(pEngraveLabel);
        m_labels.append(pLevelLabel);

        pEngraveLabel->setStyleSheet("QLabel { color: red }");

        QHBoxLayout* pHLayout = new QHBoxLayout(this);
        pHLayout->addWidget(pIconLabel);
        pHLayout->addWidget(pEngraveLabel);
        pHLayout->addWidget(pLevelLabel);

        ui->vLayoutEngrave->addLayout(pHLayout);
    }
}
