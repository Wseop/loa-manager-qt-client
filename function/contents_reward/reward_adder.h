#ifndef REWARD_ADDER_H
#define REWARD_ADDER_H

#include <QWidget>

namespace Ui {
class RewardAdder;
}

class RewardAdder : public QWidget
{
    Q_OBJECT

public:
    RewardAdder(const QString& content, const QHash<QString, QStringList>& dropTable);
    ~RewardAdder();

private:
    void initIconPath();
    void initLevels();
    void initLevelSelector();
    void initInputList(QString level);

    void addInputData();

    static void tInsertData(QString content, QString level, QStringList items, QList<int> itemCounts, QString remark);

private:
    Ui::RewardAdder *ui;

    const QString& m_content;
    const QHash<QString, QStringList>& m_dropTable;
    QStringList m_levels;
    QHash<QString, QString> m_iconPaths;

    class QIntValidator* m_pInputValidator;

    QList<QWidget*> m_widgets;
    class QComboBox* m_pLevelSelector;
    class QGridLayout* m_pGridLayout;
    QList<QWidget*> m_inputWidgets;
    QList<class QLineEdit*> m_lineEdits;
};

#endif // REWARD_ADDER_H
