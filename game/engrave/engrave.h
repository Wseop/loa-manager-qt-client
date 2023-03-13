#ifndef ENGRAVE_H
#define ENGRAVE_H

#include <QString>
#include <QHash>

class Engrave
{
public:
    Engrave();

    QStringList getEngraves() const;
    QStringList getPenalties() const;
    int getEngraveValue(const QString &engrave) const;
    int getPenaltyValue(const QString &penalty) const;
    int getEngraveLevel(const QString &engrave) const;
    int getPenaltyLevel(const QString &penalty) const;

    void addEngrave(const QString &engrave, int value);
    void addPenalty(const QString &penalty, int value);
    void addEngraveLevel(const QString &engrave, int level);
    void addPenaltyLevel(const QString &penalty, int level);

private:
    QHash<QString, int> mEngraveValues;
    QHash<QString, int> mPenaltyValues;
};

#endif // ENGRAVE_H
