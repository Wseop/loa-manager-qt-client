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
        ui->vLayoutRight->addWidget(pLabel);
        mWidgets << pLabel;
        return;
    }

    initializeIcon(pBracelet->iconPath(), pBracelet->itemGrade());
    initializeEffectInfo(pBracelet->effects());
    initializeSpecialEffectInfo(pBracelet->specialEffects());
}

BraceletInfo::~BraceletInfo()
{
    for (QWidget *pWidget : mWidgets)
        delete pWidget;
    mWidgets.clear();

    for (auto rIter = mLayouts.rbegin(); rIter != mLayouts.rend(); rIter++)
        delete *rIter;
    mLayouts.clear();

    delete ui;
}

void BraceletInfo::initializeIcon(const QString &iconPath, ItemGrade itemGrade)
{
    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    QLabel *pIcon = WidgetManager::createIcon(iconPath, pNetworkManager, itemGradeToBGColor(itemGrade));
    ui->vLayoutLeft->addWidget(pIcon);
    mWidgets << pIcon;
}

void BraceletInfo::initializeEffectInfo(const QList<QPair<QString, int> > &effects)
{
    for (const QPair<QString, int> &effect : effects)
    {
        QHBoxLayout *pHLayout = createHLayout();
        QString text = QString("%1 +%2").arg(effect.first).arg(effect.second);

        QLabel *pLabelEffect = WidgetManager::createLabel(text);
        pHLayout->addWidget(pLabelEffect);
        mWidgets << pLabelEffect;
    }
}

void BraceletInfo::initializeSpecialEffectInfo(const QList<QPair<QString, int> > &specialEffects)
{
    QHBoxLayout *pHLayout = createHLayout();

    for (const QPair<QString, int> &specialEffect : specialEffects)
    {
        QLabel *pLabelEffect = WidgetManager::createLabel(specialEffect.first);
        pLabelEffect->setStyleSheet("QLabel { border: 1px solid black; border-radius: 5px; padding: 2px }");
        pHLayout->addWidget(pLabelEffect);
        mWidgets << pLabelEffect;
    }
}

QHBoxLayout *BraceletInfo::createHLayout()
{
    QHBoxLayout *pHLayout = new QHBoxLayout();

    pHLayout->setSpacing(5);
    ui->vLayoutRight->addLayout(pHLayout);
    ui->vLayoutRight->setAlignment(pHLayout, Qt::AlignLeft);
    mLayouts << pHLayout;

    return pHLayout;
}
