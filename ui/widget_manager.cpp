#include "widget_manager.h"
#include "ui/font_manager.h"
#include <QPushButton>
#include <QLabel>

FontManager* WidgetManager::m_pFontManager = FontManager::getInstance();

QPushButton* WidgetManager::createPushButton(QString text, int width, int height, int fontSize, QWidget* pParent)
{
    QPushButton* pButton = new QPushButton(text, pParent);
    pButton->setFixedSize(width, height);
    pButton->setFont(m_pFontManager->getFont(FontFamily::NanumSquareNeoBold, fontSize));
    return pButton;
}

QLabel* WidgetManager::createLabel(QString text, int width, int height, int fontSize, QWidget *pParent)
{
    QLabel* pLabel = new QLabel(text, pParent);
    pLabel->setFixedSize(width, height);
    pLabel->setFont(m_pFontManager->getFont(FontFamily::NanumSquareNeoBold, fontSize));
    return pLabel;
}
