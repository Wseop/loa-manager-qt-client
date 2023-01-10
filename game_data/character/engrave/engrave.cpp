#include "engrave.h"

Engrave::Engrave()
{

}

const QMap<QString, int>& Engrave::getEngraves() const
{
    return m_engraves;
}

const QMap<QString, int>& Engrave::getPenalties() const
{
    return m_penalties;
}

void Engrave::addEngrave(const QString& engrave, int value)
{
    m_engraves[engrave] += value;
}

void Engrave::addPenalty(const QString& penalty, int value)
{
    m_penalties[penalty] += value;
}
