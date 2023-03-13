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

    QStringList mContents;
    QHash<QString, QStringList> mContentLevels;
    QHash<QString, QStringList> mDropTable;
    QList<int> mStages;
    QHash<QString, int> mItemIndex;

    class QComboBox *mpContentSelector;
    QList<QComboBox*> mLevelSelectors;
    QComboBox *mpCurrentLevelSelector;

    class QIntValidator *mpInputValidator;
    QList<class QLineEdit*> mLineEdits;

    QList<QWidget*> mWidgets;
    QList<QLayout*> mLayouts;
};

#endif // CONTENT_REWARD_ADDER_H
