#include "widget_manager.h"
#include "ui/font_manager.h"
#include <QPushButton>

QPushButton* WidgetManager::createPushButton(QString text, int width, int height, int fontSize, QWidget* pParent)
{
    QPushButton* pButton = new QPushButton(text, pParent);
    pButton->setFixedSize(width, height);
    pButton->setFont(FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, fontSize));
    return pButton;
}
