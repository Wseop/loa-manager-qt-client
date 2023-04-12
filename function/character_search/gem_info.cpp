#include "gem_info.h"
#include "ui_gem_info.h"
#include "ui/widget_manager.h"

#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkReply>

GemInfo::GemInfo(const Gem *pGem) :
    ui(new Ui::GemInfo)
{
    ui->setupUi(this);

    initializeLayout1(pGem);
    initializeLayout2(pGem);
}

GemInfo::~GemInfo()
{
    delete ui;
}

void GemInfo::initializeLayout1(const Gem *pGem)
{
    addGemIcon(pGem->iconPath(), pGem->itemGrade());
    addGemLevelInfo(pGem->gemLevel(), pGem->itemGrade());
}

void GemInfo::addGemIcon(const QString &iconPath, ItemGrade itemGrade)
{
    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    QLabel *pIcon = WidgetManager::createIcon(iconPath, pNetworkManager, itemGradeToBGColor(itemGrade));
    ui->vLayout1->addWidget(pIcon);
}

void GemInfo::addGemLevelInfo(int gemLevel, ItemGrade itemGrade)
{
    const QString text = QString("Lv.%1").arg(gemLevel);

    QLabel *pLabelGemLevel = WidgetManager::createLabel(text, 10, "", 50);
    pLabelGemLevel->setStyleSheet(QString("QLabel { border: 1px solid black; "
                                          "         border-radius: 5px; "
                                          "         padding: 2px; "
                                          "         color: %1 }").arg(itemGradeToTextColor(itemGrade)));
    ui->vLayout1->addWidget(pLabelGemLevel);
}

void GemInfo::initializeLayout2(const Gem *pGem)
{
    addGemTypeInfo(pGem->gemType(), pGem->itemGrade());
    addSkillNameInfo(pGem->skillName());
}

void GemInfo::addGemTypeInfo(GemType gemType, ItemGrade itemGrade)
{
    QString text;

    if (gemType == GemType::멸화)
        text = "멸화";
    else if (gemType == GemType::홍염)
        text = "홍염";
    else
        return;

    QLabel *pLabelGemType = WidgetManager::createLabel(text, 10, itemGradeToTextColor(itemGrade));
    ui->vLayout2->addWidget(pLabelGemType);
    ui->vLayout2->setAlignment(pLabelGemType, Qt::AlignLeft);
}

void GemInfo::addSkillNameInfo(const QString &skillName)
{
    QLabel *pLabelSkillName = WidgetManager::createLabel(skillName);
    ui->vLayout2->addWidget(pLabelSkillName);
}
