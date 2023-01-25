#include "class_selector.h"
#include "ui_class_selector.h"
#include "ui/widget_manager.h"
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
    QFile file(":/json/json/class.json");
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << Q_FUNC_INFO << ": class.json open fail";
        return;
    }
    m_jArrClass = QJsonDocument::fromJson(file.readAll()).array();
    file.close();
}

void ClassSelector::addWidgets()
{
    const int WIDGET_WIDTH = 100;
    const int WIDGET_HEIGHT = 50;

    for (const QJsonValue& jValClass : m_jArrClass)
    {
        const QJsonObject& jObjClass = jValClass.toObject();

        // 뿌리클래스마다 layout 추가
        QHBoxLayout* pHLayout = new QHBoxLayout();
        pHLayout->setAlignment(Qt::AlignLeft);
        m_hLayouts.append(pHLayout);
        ui->vLayoutClassSelector->addLayout(pHLayout);

        // 뿌리클래스 label 추가
        const QString& parentClass = jObjClass.find("Parent")->toString();
        QLabel* pLabelParentClass = WidgetManager::createLabel(parentClass, WIDGET_WIDTH, WIDGET_HEIGHT, 12, this);
        m_labels.append(pLabelParentClass);
        pHLayout->addWidget(pLabelParentClass);

        // 자식클래스 button 추가
        const QJsonArray& jArrChildClass = jObjClass.find("Child")->toArray();
        for (const QJsonValue& jValChildClass : jArrChildClass)
        {
            const QString& childClass = jValChildClass.toString();
            QPushButton* pBtnChildClass = WidgetManager::createPushButton(childClass, WIDGET_WIDTH, WIDGET_HEIGHT, 10, this);
            m_buttons.append(pBtnChildClass);
            pHLayout->addWidget(pBtnChildClass);
        }
    }
}
