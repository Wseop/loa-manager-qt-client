#include "bracelet_info.h"
#include "ui_bracelet_info.h"
#include "ui/widget_manager.h"

#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkReply>

BraceletInfo::BraceletInfo(const Equipment &bracelet) :
    ui(new Ui::BraceletInfo)
{
    ui->setupUi(this);

    if (bracelet.type == "")
    {
        QLabel *pLabel = WidgetManager::createLabel("팔찌 미착용");
        ui->vLayout2->addWidget(pLabel);
        return;
    }

    initializeLayout1(bracelet.iconPath, bracelet.itemGrade);
    initializeLayout2(bracelet.braceletEffects);
}

BraceletInfo::~BraceletInfo()
{
    delete ui;
}

void BraceletInfo::initializeLayout1(const QString &iconPath, const ItemGrade &itemGrade)
{
    addBraceletIcon(iconPath, itemGrade);
}

void BraceletInfo::addBraceletIcon(const QString &iconPath, ItemGrade itemGrade)
{
    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished,
            pNetworkManager, &QNetworkAccessManager::deleteLater);

    QLabel *pIcon = WidgetManager::createIcon(
        iconPath, pNetworkManager, itemGradeToBGColor(itemGrade));

    ui->vLayout1->addWidget(pIcon);
}

void BraceletInfo::initializeLayout2(const QStringList &effects)
{
    QStringList normalEffects;
    QStringList specialEffects;

    for (const QString &effect : effects) {
        if (effect.startsWith("[")) {
            specialEffects << effect;
        } else {
            normalEffects << effect;
        }
    }

    addEffectInfo(normalEffects);
    addSpecialEffectInfo(specialEffects);
}

void BraceletInfo::addEffectInfo(const QStringList &effects)
{
    for (const QString &effect : effects)
    {
        QHBoxLayout *pHLayout = createHLayout(ui->vLayout2);
        QLabel *pLabelEffect = WidgetManager::createLabel(effect);
        pHLayout->addWidget(pLabelEffect);
    }
}

void BraceletInfo::addSpecialEffectInfo(const QStringList &specialEffects)
{
    QHBoxLayout *pHLayout = createHLayout(ui->vLayout2);

    for (const QString &specialEffect : specialEffects)
    {
        QLabel *pLabelEffect = WidgetManager::createLabel(specialEffect);
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
