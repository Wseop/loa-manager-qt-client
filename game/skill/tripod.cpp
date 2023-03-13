#include "tripod.h"

Tripod::Tripod()
{

}

QString Tripod::tripodName() const
{
    return mTripodName;
}

void Tripod::setTripodName(const QString &newTripodName)
{
    mTripodName = newTripodName;
}

int Tripod::tier() const
{
    return mTier;
}

void Tripod::setTier(int newTier)
{
    mTier = newTier;
}

int Tripod::tripodCode() const
{
    return mTripodCode;
}

void Tripod::setTripodCode(int newTripodCode)
{
    mTripodCode = newTripodCode;
}

int Tripod::maxLevel() const
{
    return mMaxLevel;
}

void Tripod::setMaxLevel(int newMaxLevel)
{
    mMaxLevel = newMaxLevel;
}

QString Tripod::iconPath() const
{
    return mIconPath;
}

void Tripod::setIconPath(const QString &newIconPath)
{
    mIconPath = newIconPath;
}

int Tripod::tripodLevel() const
{
    return mTripodLevel;
}

void Tripod::setTripodLevel(int newTripodLevel)
{
    mTripodLevel = newTripodLevel;
}
