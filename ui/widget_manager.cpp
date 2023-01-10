#include "widget_manager.h"
#include <QPushButton>

QPushButton* WidgetManager::createPushButton(QString text, int width, int height)
{
    QPushButton* pButton = new QPushButton(text);
    pButton->setFixedSize(width, height);
    return pButton;
}
