#include "equip.h"

Equip::Equip(ItemType type) :
    Item(type),
    m_quality(0),
    m_setEffect(SetEffect::Size),
    m_setLevel(0),
    m_bElla(false)
{

}

const QString& Equip::getLevelTier() const
{
    return m_levelTier;
}

const int& Equip::getQuality() const
{
    return m_quality;
}

const SetEffect& Equip::getSetEffect() const
{
    return m_setEffect;
}

const int& Equip::getSetLevel() const
{
    return m_setLevel;
}

bool Equip::isElla() const
{
    return m_bElla;
}

void Equip::setLevelTier(const QString& levelTier)
{
    m_levelTier = levelTier;
}

void Equip::setQuality(int quality)
{
    m_quality = quality;
}

void Equip::setSetEffect(SetEffect setEffect)
{
    m_setEffect = setEffect;
}

void Equip::setSetLevel(int setLevel)
{
    m_setLevel = setLevel;
}

void Equip::setElla(bool bElla)
{
    m_bElla = bElla;
}
