#include "reforge_item.h"
#include "ui_reforge_item.h"
#include "ui/widget_manager.h"
#include <QLabel>

ReforgeItem::ReforgeItem(QString name, QString iconPath, ItemGrade grade) :
    ui(new Ui::ReforgeItem),
    m_pIcon(nullptr),
    m_pLabelName(nullptr),
    m_pLabelPrice(nullptr)
{
    ui->setupUi(this);

    addIcon(iconPath, grade);
    addLabels(name, grade);
}

ReforgeItem::~ReforgeItem()
{
    delete m_pIcon;
    delete m_pLabelName;
    delete m_pLabelPrice;
    delete ui;
}

void ReforgeItem::setPrice(int price)
{
    m_pLabelPrice->setText(QString("[%L1골]").arg(price));
}

void ReforgeItem::addIcon(QString iconPath, ItemGrade grade)
{
    m_pIcon = WidgetManager::createIcon(iconPath, nullptr, backgroundColorCode(grade));
    ui->vLayoutReforgeItem->addWidget(m_pIcon);
    ui->vLayoutReforgeItem->setAlignment(m_pIcon, Qt::AlignHCenter);
}

void ReforgeItem::addLabels(QString name, ItemGrade grade)
{
    const int LABEL_WIDTH = 150;

    // name label 추가
    m_pLabelName = WidgetManager::createLabel(name, 10, colorCode(grade), LABEL_WIDTH);
    ui->vLayoutReforgeItem->addWidget(m_pLabelName);
    ui->vLayoutReforgeItem->setAlignment(m_pLabelName, Qt::AlignHCenter);

    // price label 추가
    m_pLabelPrice = WidgetManager::createLabel("[-]", 10, "", LABEL_WIDTH);
    ui->vLayoutReforgeItem->addWidget(m_pLabelPrice);
    ui->vLayoutReforgeItem->setAlignment(m_pLabelPrice, Qt::AlignHCenter);
}
