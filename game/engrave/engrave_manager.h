#ifndef ENGRAVE_MANAGER_H
#define ENGRAVE_MANAGER_H

#include <QStringList>
#include <QHash>

class EngraveManager
{
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
    QStringList getClassEngraves() const;
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
    QStringList mClassEngraves;
    QStringList mPenalties;
};

#endif // ENGRAVE_MANAGER_H
