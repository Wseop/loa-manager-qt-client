#include "widget_manager.h"
#include "ui/font_manager.h"

#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QComboBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QPixmap>

QPushButton* WidgetManager::createPushButton(QString text, int fontSize, int width, int height)
{
    QPushButton* pButton = new QPushButton(text);
    pButton->setFixedSize(width, height);
    pButton->setFont(FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, fontSize));
    return pButton;
}

QPushButton* WidgetManager::createPushButton(QPixmap icon, int width, int height)
{
    QPushButton* pButton = new QPushButton();
    pButton->setIcon(icon);
    pButton->setFixedSize(width, height);
    return pButton;
}

QLabel* WidgetManager::createLabel(QString text, int fontSize, QString color, int width, int height)
{
    QLabel* pLabel = new QLabel(text);
    pLabel->setMaximumSize(width, height);
    pLabel->setFont(FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, fontSize));
    pLabel->setStyleSheet(QString("QLabel { color: %1 }").arg(color));
    pLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    return pLabel;
}

QLabel* WidgetManager::createIcon(QString iconPath, QNetworkAccessManager* pNetworkManager, QString backgroundColor, int width, int height)
{
    QLabel* pIcon = new QLabel();
    pIcon->setFixedSize(width, height);
    pIcon->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    QString style = "QLabel { background-color: %1; border: 1px solid black }";

    if (pNetworkManager == nullptr)
    {
        QPixmap iconImage;
        // if network manager is nullptr, then load icon from resource
        if (iconImage.load(iconPath))
        {
            pIcon->setPixmap(iconImage.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            pIcon->setStyleSheet(style.arg(backgroundColor));
        }
    }
    else
    {
        QNetworkRequest request;
        request.setUrl(QUrl(iconPath));
        connect(pNetworkManager, &QNetworkAccessManager::finished, [&, pIcon, width, height, style, backgroundColor](QNetworkReply* pReply){
            QPixmap iconImage;
            if (iconImage.loadFromData(pReply->readAll(), "PNG"))
            {
                pIcon->setPixmap(iconImage.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
                pIcon->setStyleSheet(style.arg(backgroundColor));
            }
        });
        pNetworkManager->get(request);
    }

    return pIcon;
}

QProgressBar* WidgetManager::createQualityBar(int quality, int width, int height, int fontSize)
{
    QProgressBar* pQualityBar = new QProgressBar();
    pQualityBar->setValue(quality);
    pQualityBar->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    pQualityBar->setFormat("%p");
    pQualityBar->setFixedSize(width, height);
    pQualityBar->setFont(FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, fontSize));
    pQualityBar->setStyleSheet(QString("QProgressBar::chunk { background-color: %1 }").arg(getQualityColor(quality)));
    return pQualityBar;
}

QComboBox *WidgetManager::createComboBox(QStringList items, int fontSize, int width, int height)
{
    QComboBox* pComboBox = new QComboBox();
    pComboBox->addItems(items);
    pComboBox->setFont(FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, fontSize));
    pComboBox->setMaximumSize(width, height);
    return pComboBox;
}

QLineEdit* WidgetManager::createLineEdit(QValidator* pValidator, QString placeHolder, int fontSize, int width, int height)
{
    QLineEdit* pLineEdit = new QLineEdit();
    pLineEdit->setValidator(pValidator);
    pLineEdit->setPlaceholderText(placeHolder);
    pLineEdit->setAlignment(Qt::AlignHCenter);
    pLineEdit->setFont(FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, fontSize));
    pLineEdit->setMaximumSize(width, height);
    return pLineEdit;
}

QGroupBox* WidgetManager::createGroupBox(QString title, int fontSize)
{
    QGroupBox* pGroupBox = new QGroupBox(title);
    pGroupBox->setFont(FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, fontSize));
    return pGroupBox;
}

QFrame* WidgetManager::createLine(QFrame::Shape direction)
{
    QFrame* pLine = new QFrame();
    pLine->setFrameStyle(direction);
    return pLine;
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
