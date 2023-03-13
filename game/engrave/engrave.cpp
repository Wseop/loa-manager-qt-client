#include "engrave.h"

const int VALUE_PER_LEVEL = 5;

Engrave::Engrave()
{

}

QStringList Engrave::getEngraves() const
{
    return mEngraveValues.keys();
}

QStringList Engrave::getPenalties() const
{
    return mPenaltyValues.keys();
}

int Engrave::getEngraveValue(const QString &engrave) const
{
    return mEngraveValues[engrave];
}

int Engrave::getPenaltyValue(const QString &penalty) const
{
    return mPenaltyValues[penalty];
}

int Engrave::getEngraveLevel(const QString &engrave) const
{
    return mEngraveValues[engrave] / VALUE_PER_LEVEL;
}

int Engrave::getPenaltyLevel(const QString &penalty) const
{
    return mPenaltyValues[penalty] / VALUE_PER_LEVEL;
}

void Engrave::addEngrave(const QString &engrave, int value)
{
    mEngraveValues[engrave] += value;
}

void Engrave::addPenalty(const QString &penalty, int value)
{
    mPenaltyValues[penalty] += value;
}

void Engrave::addEngraveLevel(const QString &engrave, int level)
{
    mEngraveValues[engrave] += level * VALUE_PER_LEVEL;
}

void Engrave::addPenaltyLevel(const QString &penalty, int level)
{
    mPenaltyValues[penalty] += level * VALUE_PER_LEVEL;
}

