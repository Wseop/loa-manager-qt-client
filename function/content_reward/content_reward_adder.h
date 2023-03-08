#ifndef CONTENT_REWARD_ADDER_H
#define CONTENT_REWARD_ADDER_H

#include "db/db_enums.h"

#include <QWidget>

namespace Ui {
class ContentRewardAdder;
}

class ContentRewardAdder : public QWidget
{
    Q_OBJECT

public:
    ContentRewardAdder(const QStringList &contents, const QHash<QString, QStringList> &contentLevels, const QHash<QString, QStringList> &dropTable);
    ~ContentRewardAdder();

private:
    void initializeContentSelector();
    void initializeLevelSelector();
    void initializeInsertButton();
    void initializeDataInputTable();

    void insertData();

private:
    static void tInsertData(Collection collection, int count, QString level, QStringList items, QList<int> itemCounts);

private:
    Ui::ContentRewardAdder *ui;

    QStringList m_contents;
    QHash<QString, QStringList> m_contentLevels;
    QHash<QString, QStringList> m_dropTable;
    QList<int> m_stages;
    QHash<QString, int> m_itemIndex;

    class QComboBox *m_pContentSelector;
    QList<QComboBox*> m_levelSelectors;
    QComboBox *m_pCurrentLevelSelector;

    class QIntValidator *m_pInputValidator;
    QList<class QLineEdit*> m_lineEdits;

    QList<QWidget*> m_widgets;
    QList<QLayout*> m_layouts;
};

#endif // CONTENT_REWARD_ADDER_H
