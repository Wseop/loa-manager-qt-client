#ifndef REWARD_WIDGET_H
#define REWARD_WIDGET_H

#include <QWidget>

enum class ContentType
{
    Chaos,
    Guardian,
};

namespace Ui {
class RewardWidget;
}

class RewardWidget : public QWidget
{
    Q_OBJECT

public:
    RewardWidget(ContentType type, QString level, int levelCount, QStringList items, QList<QJsonObject> data);
    ~RewardWidget();

private:
    void initIconPath();
    void initTitle();
    void initData(const QList<QJsonObject>& data);

private:
    Ui::RewardWidget *ui;

    ContentType m_type;
    QString m_level;
    int m_levelCount;
    QStringList m_items;
    QHash<QString, QList<int>> m_data;
    QHash<QString, int> m_dataCount;
    QHash<QString, QString> m_iconPaths;

    // widgets
    QList<QWidget*> m_widgets;
};

#endif // REWARD_WIDGET_H
