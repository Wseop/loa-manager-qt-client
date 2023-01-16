#include "widget_manager.h"
#include "ui/font_manager.h"
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>

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

QProgressBar* WidgetManager::createQualityBar(int quality, int width, int height, int fontSize, QWidget* pParent)
{
    QProgressBar* pQualityBar = new QProgressBar(pParent);
    pQualityBar->setValue(quality);
    pQualityBar->setAlignment(Qt::AlignHCenter);
    pQualityBar->setFormat("%p");
    pQualityBar->setFixedSize(width, height);
    pQualityBar->setFont(m_pFontManager->getFont(FontFamily::NanumSquareNeoBold, fontSize));
    pQualityBar->setStyleSheet(QString("QProgressBar::chunk { background-color: %1 }").arg(getQualityColor(quality)));
    return pQualityBar;
}

QString WidgetManager::getQualityColor(int quality)
{
    QString color;

        if (quality == 100)
            color = "#FE9600";
        else if (quality >= 90)
            color = "#CE43FC";
        else if (quality >= 70)
            color = "#00B5FF";
        else if (quality >= 30)
            color = "#00B700";
        else if (quality >= 10)
            color = "#FFD200";
        else
            color = "#FF6000";

        return color;
}
