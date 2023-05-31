#include "gem_info.h"
#include "ui_gem_info.h"
#include "ui/widget_manager.h"

#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkReply>

GemInfo::GemInfo(const Gem &gem) :
    ui(new Ui::GemInfo)
{
    ui->setupUi(this);

    initializeLayout1(gem.iconPath, gem.itemGrade, gem.gemLevel);
    initializeLayout2(gem.type, gem.itemGrade, gem.skillName);
}

GemInfo::~GemInfo()
{
    delete ui;
}

void GemInfo::initializeLayout1(const QString &iconPath, const ItemGrade &itemGrade, int gemLevel)
{
    addGemIcon(iconPath, itemGrade);
    addGemLevelInfo(gemLevel, itemGrade);
}

void GemInfo::addGemIcon(const QString &iconPath, ItemGrade itemGrade)
{
    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished,
            pNetworkManager, &QNetworkAccessManager::deleteLater);

    QLabel *pIcon = WidgetManager::createIcon(
        iconPath, pNetworkManager, itemGradeToBGColor(itemGrade));

    ui->vLayout1->addWidget(pIcon);
}

void GemInfo::addGemLevelInfo(int gemLevel, ItemGrade itemGrade)
{
    const QString text = QString("Lv.%1").arg(gemLevel);

    QLabel *pLabelGemLevel = WidgetManager::createLabel(
        QString("Lv.%1").arg(gemLevel), 10, 50);

    pLabelGemLevel->setStyleSheet(QString("QLabel { border: 1px solid black; "
                                          "         border-radius: 5px; "
                                          "         padding: 2px; "
                                          "         color: %1 }").arg(
                                          itemGradeToTextColor(itemGrade)));
    ui->vLayout1->addWidget(pLabelGemLevel);
}

void GemInfo::initializeLayout2(const QString &type, const ItemGrade &itemGrade, const QString &skillName)
{
    addGemTypeInfo(type, itemGrade);
    addSkillNameInfo(skillName);
}

void GemInfo::addGemTypeInfo(const QString &type, const ItemGrade &itemGrade)
{
    if (type != "멸화" && type != "홍염")
        return;

    QLabel *pLabelGemType = WidgetManager::createLabel(type);
    pLabelGemType->setStyleSheet(
        QString("QLabel { color: %1 }").arg(itemGradeToTextColor(itemGrade)));

    ui->vLayout2->addWidget(pLabelGemType);
    ui->vLayout2->setAlignment(pLabelGemType, Qt::AlignLeft);
}

void GemInfo::addSkillNameInfo(const QString &skillName)
{
    QLabel *pLabelSkillName = WidgetManager::createLabel(skillName);
    ui->vLayout2->addWidget(pLabelSkillName);
}
