#ifndef ENGRAVE_MANAGER_H
#define ENGRAVE_MANAGER_H

#include <QStringList>
#include <QMap>

class EngraveManager
{
private:
    EngraveManager();
    ~EngraveManager();

public:
    static EngraveManager* getInstance();
    static void destroyInstance();

    QStringList getEngraves() const;
    const QStringList& getNormalEngraves() const;
    const QStringList& getClassEngraves() const;
    const QStringList& getPenalties() const;
    QString iconPath(const QString& engrave);

private:
    static EngraveManager* m_pEngrave;

    QMap<QString, int> m_engraveToCode;
    QStringList m_normalEngraves;
    QStringList m_classEngraves;
    QStringList m_penalties;
};

#endif // ENGRAVE_MANAGER_H
