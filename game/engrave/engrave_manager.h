#ifndef ENGRAVE_MANAGER_H
#define ENGRAVE_MANAGER_H

#include <QObject>
#include <QStringList>
#include <QHash>
#include <QEventLoop>

class EngraveManager : public QObject
{
    Q_OBJECT

private:
    EngraveManager();
    ~EngraveManager();

    void initalizeEngraveList();

public:
    static EngraveManager *getInstance();
    static void destroyInstance();

public:
    QStringList getEngraves() const;
    QStringList getBattleEngraves() const;
    QStringList getClassEngraves(const QString &characterClass) const;
    QStringList getAllClassEngraves() const;
    QStringList getPenalties() const;

    const QString iconPath(const QString &engrave) const;

    bool isClassEngrave(const QString &engrave);
    bool isPenalty(const QString &engrave);

    int getEngraveCode(const QString &engrave);
    const QString getEngraveByCode(const int &code) const;

private:
    static EngraveManager *mpEngrave;

private:
    QHash<QString, int> mEngraveToCode;
    QHash<int, QString> mCodeToEngrave;

    QStringList mBattleEngraves;
    QHash<QString, QStringList> mClassEngraves;
    QStringList mPenalties;

    QEventLoop mEventLoop;
};

#endif // ENGRAVE_MANAGER_H
