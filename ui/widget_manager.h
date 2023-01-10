#ifndef WIDGETMANAGER_H
#define WIDGETMANAGER_H

#include <QObject>

class WidgetManager : public QObject
{
    Q_OBJECT

public:
    static class QPushButton* createPushButton(QString text, int width, int height);
};

#endif // WIDGETMANAGER_H
