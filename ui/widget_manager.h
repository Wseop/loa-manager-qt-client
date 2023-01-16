#ifndef WIDGETMANAGER_H
#define WIDGETMANAGER_H

#include <QObject>

class QLabel;
class QPushButton;
class QProgressBar;
class QNetworkAccessManager;

class WidgetManager : public QObject
{
    Q_OBJECT

public:
    static QPushButton* createPushButton(QString text, int width, int height, int fontSize = 10, QWidget* pParent = nullptr);
    static QLabel* createLabel(QString text, int width, int height, int fontSize = 10, QWidget* pParent = nullptr);
    static QLabel* createIcon(QString iconPath, QNetworkAccessManager* pNetworkManager, QWidget* pParent = nullptr);
    static QProgressBar* createQualityBar(int quality, int width, int height, int fontSize = 10, QWidget* pParent = nullptr);

private:
    static QString getQualityColor(int quality);

private:
    static class FontManager* m_pFontManager;
};

#endif // WIDGETMANAGER_H
