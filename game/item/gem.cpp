#include "gem.h"

Gem::Gem(GemType gemType) :
    Item(ItemType::Gem),
    m_gemType(gemType)
{

}

GemType Gem::gemType() const
{
    return m_gemType;
}

int Gem::gemLevel() const
{
    return m_gemLevel;
}

void Gem::setGemLevel(int newGemLevel)
{
    m_gemLevel = newGemLevel;
}
