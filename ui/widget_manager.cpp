#include "widget_manager.h"
#include "ui/font_manager.h"
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QPixmap>

QPushButton* WidgetManager::createPushButton(QString text, int width, int height, int fontSize, QWidget* pParent)
{
    QPushButton* pButton = new QPushButton(text, pParent);
    pButton->setFixedSize(width, height);
    pButton->setFont(FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, fontSize));
    return pButton;
}

QLabel* WidgetManager::createLabel(QString text, int width, int height, int fontSize, QWidget *pParent, QString color)
{
    QLabel* pLabel = new QLabel(text, pParent);
    pLabel->setFixedSize(width, height);
    pLabel->setFont(FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, fontSize));
    pLabel->setStyleSheet(QString("QLabel { color: %1 }").arg(color));
    pLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    return pLabel;
}

QLabel* WidgetManager::createIcon(QString iconPath, QNetworkAccessManager* pNetworkManager, int width, int height, QWidget* pParent)
{
    QLabel* pIcon = new QLabel(pParent);
    pIcon->setFixedSize(width, height);

    if (pNetworkManager == nullptr)
    {
        QPixmap iconImage;
        // if network manager is nullptr, then load icon from resource
        if (iconImage.load(iconPath))
        {
            pIcon->setPixmap(iconImage.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            pIcon->setStyleSheet("QLabel { border: 1px solid black }");
        }
    }
    else
    {
        QNetworkRequest request;
        request.setUrl(QUrl(iconPath));
        connect(pNetworkManager, &QNetworkAccessManager::finished, [&, pIcon, width, height](QNetworkReply* pReply){
            QPixmap iconImage;
            if (iconImage.loadFromData(pReply->readAll(), "PNG"))
            {
                pIcon->setPixmap(iconImage.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
                pIcon->setStyleSheet("QLabel { border: 1px solid black }");
            }
        });
        pNetworkManager->get(request);
    }

    return pIcon;
}

QProgressBar* WidgetManager::createQualityBar(int quality, int width, int height, int fontSize, QWidget* pParent)
{
    QProgressBar* pQualityBar = new QProgressBar(pParent);
    pQualityBar->setValue(quality);
    pQualityBar->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    pQualityBar->setFormat("%p");
    pQualityBar->setFixedSize(width, height);
    pQualityBar->setFont(FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, fontSize));
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
