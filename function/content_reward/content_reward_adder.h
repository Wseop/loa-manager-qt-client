#ifndef CONTENT_REWARD_ADDER_H
#define CONTENT_REWARD_ADDER_H

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

    void processReply(class QNetworkReply *pReply);

private:
    Ui::ContentRewardAdder *ui;

    QStringList mContents;
    QHash<QString, QStringList> mContentLevels;
    QHash<QString, QStringList> mDropTable;
    QHash<QString, int> mItemIndex;

    class QComboBox *mpContentSelector;
    QList<QComboBox*> mLevelSelectors;
    QComboBox *mpCurrentLevelSelector;

    class QIntValidator *mpInputValidator;
    QList<class QLineEdit*> mLineEdits;
};

#endif // CONTENT_REWARD_ADDER_H
