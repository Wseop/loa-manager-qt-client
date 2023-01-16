#ifndef WIDGETMANAGER_H
#define WIDGETMANAGER_H

#include <QObject>

class WidgetManager : public QObject
{
    Q_OBJECT

public:
    static class QPushButton* createPushButton(QString text, int width, int height, int fontSize = 10, QWidget* pParent = nullptr);
    static class QLabel* createLabel(QString text, int width, int height, int fontSize = 10, QWidget* pParent = nullptr);
    static class QProgressBar* createQualityBar(int quality, int width, int height, int fontSize = 10, QWidget* pParent = nullptr);

private:
    static QString getQualityColor(int quality);

private:
    static class FontManager* m_pFontManager;
};

#endif // WIDGETMANAGER_H
