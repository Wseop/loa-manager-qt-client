#include "collectible.h"
#include "resource/resource_manager.h"

Collectible::Collectible(QString type, int point, int maxPoint) :
    mType(type),
    mPoint(point),
    mMaxPoint(maxPoint)
{
    mIconPath = ResourceManager::getInstance()->iconPath(type);
}

QString Collectible::getType() const
{
    return mType;
}

QString Collectible::getIconPath() const
{
    return mIconPath;
}

int Collectible::getPoint() const
{
    return mPoint;
}

int Collectible::getMaxPoint() const
{
    return mMaxPoint;
}
