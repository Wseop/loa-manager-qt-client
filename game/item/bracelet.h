#ifndef BRACELET_H
#define BRACELET_H

#include "game/item/item.h"

#include <QStringList>

class Bracelet : public Item
{
public:
    Bracelet();

    QList<QPair<QString, int>> effects() const;
    void addEffect(const QString &effect, int value);

    QList<QPair<QString, int>> specialEffects() const;
    void addSpecialEffect(const QString &effect, int value);

private:
    QList<QPair<QString, int>> m_effects;
    QList<QPair<QString, int>> m_specialEffects;
};

#endif // BRACELET_H
