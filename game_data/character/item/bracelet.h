#ifndef BRACELET_H
#define BRACELET_H

#include "game_data/character/item/item.h"

class Bracelet : public Item
{
public:
    Bracelet();

    const QStringList& getEffects() const;
    const QStringList& getSpecialEffects() const;

    void addEffect(const QString& effect);

private:
    QStringList m_effects;
    QStringList m_specialEffects;
};

#endif // BRACELET_H
