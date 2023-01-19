#ifndef ENGRAVE_H
#define ENGRAVE_H

#include <QString>
#include <QMap>

using PairEngraveValue = QPair<QString, int>;

class Engrave
{
public:
    Engrave();

    const QList<PairEngraveValue>& getEngraves() const;
    const QList<PairEngraveValue>& getPenalties() const;

    void addEngrave(const QString& engrave, int value);
    void addPenalty(const QString& penalty, int value);

private:
    QList<PairEngraveValue> m_engraves;
    QList<PairEngraveValue> m_penalties;
};

#endif // ENGRAVE_H
