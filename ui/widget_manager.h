#ifndef WIDGETMANAGER_H
#define WIDGETMANAGER_H

#include <QObject>

class QLabel;
class QPushButton;
class QProgressBar;
class QComboBox;
class QNetworkAccessManager;
class QLineEdit;
class QGroupBox;

class WidgetManager : public QObject
{
    Q_OBJECT

public:
    static QPushButton* createPushButton(QString text, int fontSize = 10, int width = 100, int height = 50);
    static QPushButton* createPushButton(QPixmap icon, int fontSize = 10, int width = 100, int height = 50);
    static QLabel* createLabel(QString text, int fontSize = 10, QString color = "", int width = 100, int height = 25);
    static QLabel* createIcon(QString iconPath, QNetworkAccessManager* pNetworkManager, QString backgroundColor = "", int width = 50, int height = 50);
    static QProgressBar* createQualityBar(int quality, int width, int height, int fontSize = 10);
    static QComboBox* createComboBox(QStringList items, int fontSize = 10, int width = 100, int height = 25);
    static QLineEdit* createLineEdit(class QValidator* pValidator, QString placeHolder = "", int fontSize = 10, int width = 100, int height = 25);
    static QGroupBox* createGroupBox(QString title, int fontSize = 10);

private:
    static QString getQualityColor(int quality);
};

#endif // WIDGETMANAGER_H
