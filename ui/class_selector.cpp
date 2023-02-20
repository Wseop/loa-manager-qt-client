#include "class_selector.h"
#include "ui_class_selector.h"
#include "ui/widget_manager.h"
#include "resource/resource_manager.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

ClassSelector::ClassSelector() :
    ui(new Ui::ClassSelector)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icon/Home.ico"));
    this->setWindowTitle("직업 선택");

    loadClass();
    addWidgets();
}

ClassSelector::~ClassSelector()
{
    delete ui;
}

QList<QPushButton*>& ClassSelector::getButtons()
{
    return m_buttons;
}

void ClassSelector::loadClass()
{
    const QJsonObject json = ResourceManager::getInstance()->loadJson("class");
    m_arrClass = json.find("Class")->toArray();
}

void ClassSelector::addWidgets()
{
    const int WIDGET_WIDTH = 100;
    const int WIDGET_HEIGHT = 50;

    for (const QJsonValue& valueClass : m_arrClass)
    {
        const QJsonObject& objClass = valueClass.toObject();

        // 뿌리클래스마다 layout 추가
        QHBoxLayout* pHLayout = new QHBoxLayout();
        pHLayout->setAlignment(Qt::AlignLeft);
        m_hLayouts.append(pHLayout);
        ui->vLayoutClassSelector->addLayout(pHLayout);

        // 뿌리클래스 label 추가
        const QString& parentClass = objClass.find("Parent")->toString();
        QLabel* pLabelParentClass = WidgetManager::createLabel(parentClass, 12, "", WIDGET_WIDTH, WIDGET_HEIGHT);
        m_labels.append(pLabelParentClass);
        pHLayout->addWidget(pLabelParentClass);

        // 자식클래스 button 추가
        const QJsonArray& jArrChildClass = objClass.find("Child")->toArray();
        for (const QJsonValue& jValChildClass : jArrChildClass)
        {
            const QString& childClass = jValChildClass.toString();
            QPushButton* pBtnChildClass = WidgetManager::createPushButton(childClass, 10);
            m_buttons.append(pBtnChildClass);
            pHLayout->addWidget(pBtnChildClass);
        }
    }
}
