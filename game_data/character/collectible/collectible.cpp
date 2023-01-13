#include "collectible.h"

Collectible::Collectible() :
    m_type(CollectibleType::Size),
    m_maxPoint(0),
    m_point(0)
{

}

const CollectibleType& Collectible::getType() const
{
    return m_type;
}

const int& Collectible::getMaxPoint() const
{
    return m_maxPoint;
}

const int& Collectible::getPoint() const
{
    return m_point;
}

void Collectible::setType(const CollectibleType& type)
{
    m_type = type;
}

void Collectible::setMaxPoint(const int& maxPoint)
{
    m_maxPoint = maxPoint;
}

void Collectible::setPoint(const int& point)
{
    m_point = point;
}
