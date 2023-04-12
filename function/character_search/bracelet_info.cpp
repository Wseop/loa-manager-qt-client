#include "bracelet_info.h"
#include "ui_bracelet_info.h"
#include "ui/widget_manager.h"
#include "game/item/bracelet.h"

#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkReply>

BraceletInfo::BraceletInfo(const Bracelet *pBracelet) :
    ui(new Ui::BraceletInfo)
{
    ui->setupUi(this);

    if (pBracelet == nullptr)
    {
        QLabel *pLabel = WidgetManager::createLabel("팔찌 미착용");
        ui->vLayout2->addWidget(pLabel);
        return;
    }

    initializeLayout1(pBracelet);
    initializeLayout2(pBracelet);
}

BraceletInfo::~BraceletInfo()
{
    delete ui;
}

void BraceletInfo::initializeLayout1(const Bracelet *pBracelet)
{
    addBraceletIcon(pBracelet->iconPath(), pBracelet->itemGrade());
}

void BraceletInfo::addBraceletIcon(const QString &iconPath, ItemGrade itemGrade)
{
    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    QLabel *pIcon = WidgetManager::createIcon(iconPath, pNetworkManager, itemGradeToBGColor(itemGrade));
    ui->vLayout1->addWidget(pIcon);
}

void BraceletInfo::initializeLayout2(const Bracelet *pBracelet)
{
    addEffectInfo(pBracelet->effects());
    addSpecialEffectInfo(pBracelet->specialEffects());
}

void BraceletInfo::addEffectInfo(const QList<QPair<QString, int> > &effects)
{
    for (const QPair<QString, int> &effect : effects)
    {
        QHBoxLayout *pHLayout = createHLayout(ui->vLayout2);
        const QString text = QString("%1 +%2").arg(effect.first).arg(effect.second);

        QLabel *pLabelEffect = WidgetManager::createLabel(text);
        pHLayout->addWidget(pLabelEffect);
    }
}

void BraceletInfo::addSpecialEffectInfo(const QList<QPair<QString, int> > &specialEffects)
{
    QHBoxLayout *pHLayout = createHLayout(ui->vLayout2);

    for (const QPair<QString, int> &specialEffect : specialEffects)
    {
        QLabel *pLabelEffect = WidgetManager::createLabel(specialEffect.first);
        pLabelEffect->setStyleSheet("QLabel { border: 1px solid black; "
                                    "         border-radius: 5px; "
                                    "         padding: 2px }");
        pHLayout->addWidget(pLabelEffect);
    }
}

QHBoxLayout *BraceletInfo::createHLayout(QVBoxLayout *pLayout)
{
    QHBoxLayout *pHLayout = new QHBoxLayout();

    pHLayout->setSpacing(5);
    pLayout->addLayout(pHLayout);
    pLayout->setAlignment(pHLayout, Qt::AlignLeft);

    return pHLayout;
}
