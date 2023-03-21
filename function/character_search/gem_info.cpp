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

    initializeIcon(pGem->iconPath(), pGem->itemGrade());
    initializeGemLevelInfo(pGem->gemLevel(), pGem->itemGrade());
    initializeGemTypeInfo(pGem->gemType(), pGem->itemGrade());
    initializeSkillNameInfo(pGem->skillName());
}

GemInfo::~GemInfo()
{
    for (QWidget *pWidget : mWidgets)
        delete pWidget;
    mWidgets.clear();

    for (QLayout *pLayout : mLayouts)
        delete pLayout;
    mLayouts.clear();

    delete ui;
}

void GemInfo::initializeIcon(const QString &iconPath, ItemGrade itemGrade)
{
    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    QLabel *pIcon = WidgetManager::createIcon(iconPath, pNetworkManager, itemGradeToBGColor(itemGrade));
    ui->vLayoutLeft->addWidget(pIcon);
    mWidgets << pIcon;
}

void GemInfo::initializeGemLevelInfo(int gemLevel, ItemGrade itemGrade)
{
    QString text = QString("Lv.%1").arg(gemLevel);
    QLabel *pLabelGemLevel = WidgetManager::createLabel(text, 10, "", 50);
    pLabelGemLevel->setStyleSheet(QString("QLabel { border: 1px solid black; border-radius: 5px; padding: 2px; "
                                  "         color: %1}").arg(itemGradeToTextColor(itemGrade)));
    ui->vLayoutLeft->addWidget(pLabelGemLevel);
    mWidgets << pLabelGemLevel;
}

void GemInfo::initializeGemTypeInfo(GemType gemType, ItemGrade itemGrade)
{
    QString text;

    if (gemType == GemType::멸화)
        text = "멸화";
    else if (gemType == GemType::홍염)
        text = "홍염";
    else
        return;

    QLabel *pLabelGemType = WidgetManager::createLabel(text, 10, itemGradeToTextColor(itemGrade));
    ui->vLayoutRight->addWidget(pLabelGemType);
    ui->vLayoutRight->setAlignment(pLabelGemType, Qt::AlignLeft);
    mWidgets << pLabelGemType;
}

void GemInfo::initializeSkillNameInfo(const QString &skillName)
{
    QLabel *pLabelSkillName = WidgetManager::createLabel(skillName);
    ui->vLayoutRight->addWidget(pLabelSkillName);
    mWidgets << pLabelSkillName;
}
