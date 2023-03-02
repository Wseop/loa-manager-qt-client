#include "engrave.h"

const int VALUE_PER_LEVEL = 5;

Engrave::Engrave()
{

}

const QStringList Engrave::getEngraves() const
{
    return m_engraveValues.keys();
}

const QStringList Engrave::getPenalties() const
{
    return m_penaltyValues.keys();
}

int Engrave::getEngraveValue(const QString& engrave) const
{
    return m_engraveValues[engrave];
}

int Engrave::getPenaltyValue(const QString& penalty) const
{
    return m_penaltyValues[penalty];
}

int Engrave::getEngraveLevel(const QString& engrave) const
{
    return m_engraveValues[engrave] / VALUE_PER_LEVEL;
}

int Engrave::getPenaltyLevel(const QString& penalty) const
{
    return m_penaltyValues[penalty] / VALUE_PER_LEVEL;
}

void Engrave::addEngrave(const QString& engrave, int value)
{
    m_engraveValues[engrave] += value;
}

void Engrave::addPenalty(const QString& penalty, int value)
{
    m_penaltyValues[penalty] += value;
}

