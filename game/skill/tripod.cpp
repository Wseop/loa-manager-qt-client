#include "tripod.h"

Tripod::Tripod()
{

}

QString Tripod::tripodName() const
{
    return m_tripodName;
}

void Tripod::setTripodName(const QString &newTripodName)
{
    m_tripodName = newTripodName;
}

int Tripod::tier() const
{
    return m_tier;
}

void Tripod::setTier(int newTier)
{
    m_tier = newTier;
}

int Tripod::tripodCode() const
{
    return m_tripodCode;
}

void Tripod::setTripodCode(int newTripodCode)
{
    m_tripodCode = newTripodCode;
}

int Tripod::maxLevel() const
{
    return m_maxLevel;
}

void Tripod::setMaxLevel(int newMaxLevel)
{
    m_maxLevel = newMaxLevel;
}

QString Tripod::iconPath() const
{
    return m_iconPath;
}

void Tripod::setIconPath(const QString &newIconPath)
{
    m_iconPath = newIconPath;
}
