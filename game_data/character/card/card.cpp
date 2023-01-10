#include "card.h"

Card::Card()
{

}

const QList<QString>& Card::getEffectNames() const
{
    return m_effectNames;
}

const QList<QString>& Card::getEffectDescs() const
{
    return m_effectDescs;
}

void Card::addCardEffect(const QString& name, const QString& desc)
{
    m_effectNames.append(name);
    m_effectDescs.append(desc);
}
