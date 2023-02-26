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
    ui(new Ui::ClassSelector),
    m_classes(ResourceManager::getInstance()->loadJson("class").find("Class")->toArray())
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icon/Home.ico"));
    this->setWindowTitle("직업 선택");

    initUI();
}

ClassSelector::~ClassSelector()
{
    for (QWidget* pWidget : m_widgets)
        delete pWidget;
    for (QLayout* pLayout : m_layouts)
        delete pLayout;
    delete ui;
}

QList<QPushButton*>& ClassSelector::getButtons()
{
    // ClassSelector를 사용하는 객체에서 button의 기능을 정의하여 사용하도록 button 목록 반환
    return m_buttons;
}

void ClassSelector::initUI()
{
    const int WIDGET_WIDTH = 100;
    const int WIDGET_HEIGHT = 50;

    for (const QJsonValue& value : m_classes)
    {
        const QJsonObject& objClass = value.toObject();

        // 부모 직업마다 layout 추가
        QHBoxLayout* pHLayout = new QHBoxLayout();
        pHLayout->setAlignment(Qt::AlignLeft);
        ui->vLayoutClassSelector->addLayout(pHLayout);
        m_layouts.append(pHLayout);

        // 부모 직업 label 추가
        const QString& parentClass = objClass.find("Parent")->toString();
        QLabel* pLabelParentClass = WidgetManager::createLabel(parentClass, 12, "", WIDGET_WIDTH, WIDGET_HEIGHT);
        pHLayout->addWidget(pLabelParentClass);
        m_widgets.append(pLabelParentClass);

        // 자식 직업들 button 추가
        const QStringList& childClasses = objClass.find("Child")->toVariant().toStringList();
        for (const QString& childClass : childClasses)
        {
            QPushButton* pBtnChildClass = WidgetManager::createPushButton(childClass, 10);
            pHLayout->addWidget(pBtnChildClass);
            m_buttons.append(pBtnChildClass);
            m_widgets.append(pBtnChildClass);
        }
    }
}
