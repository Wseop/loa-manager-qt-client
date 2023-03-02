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
    static EngraveManager* getInstance();
    static void destroyInstance();

public:
    const QStringList getEngraves() const;
    const QStringList& getbattleEngraves() const;
    const QStringList& getClassEngraves() const;
    const QStringList& getPenalties() const;

    const QString iconPath(const QString& engrave) const;

    bool isClassEngrave(const QString& engrave);
    bool isPenalty(const QString& engrave);

    int getEngraveCode(const QString& engrave);
    const QString getEngraveByCode(const int& code) const;

private:
    static EngraveManager* m_pEngrave;

private:
    QHash<QString, int> m_engraveToCode;
    QHash<int, QString> m_codeToEngrave;

    QStringList m_battleEngraves;
    QStringList m_classEngraves;
    QStringList m_penalties;
};

#endif // ENGRAVE_MANAGER_H
