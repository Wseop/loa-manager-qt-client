#include "gem_widget.h"
#include "ui_gem_widget.h"
#include "game_data/character/item/gem.h"
#include "ui/widget_manager.h"
#include "ui/font_manager.h"
#include <QLabel>
#include <QNetworkAccessManager>

GemWidget::GemWidget(QWidget* pParent, const Gem* pGem) :
    QWidget(pParent),
    ui(new Ui::GemWidget),
    m_pGem(pGem),
    m_pNetworkManager(new QNetworkAccessManager())
{
    ui->setupUi(this);
    ui->groupGem->setTitle(gemTypeToStr(m_pGem->getGemType()));
    ui->vLayoutIcon->setAlignment(Qt::AlignHCenter);

    addIcon();
    addLabels();
    setFonts();
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
    QLabel* pIcon = WidgetManager::createIcon(m_pGem->getIconPath(), m_pNetworkManager, backgroundColorCode(m_pGem->getGrade()));
    m_labels.append(pIcon);
    ui->vLayoutIcon->addWidget(pIcon);
}

void GemWidget::addLabels()
{
    QLabel* pLabelLevel = WidgetManager::createLabel(QString("Lv. %1").arg(m_pGem->getLevel()), 10, colorCode(m_pGem->getGrade()), LABEL_WIDTH, LABEL_HEIGHT);
    m_labels.append(pLabelLevel);
    ui->vLayoutInfo->addWidget(pLabelLevel);

    QLabel* pLabelEffect = WidgetManager::createLabel(m_pGem->getEffect(), 10, "", LABEL_WIDTH, LABEL_HEIGHT);
    m_labels.append(pLabelEffect);
    ui->vLayoutInfo->addWidget(pLabelEffect);
}

void GemWidget::setFonts()
{
    FontManager* pFontManager = FontManager::getInstance();
    QFont nanumBold10 = pFontManager->getFont(FontFamily::NanumSquareNeoBold, 10);

    ui->groupGem->setFont(nanumBold10);
}
