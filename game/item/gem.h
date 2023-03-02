#ifndef GEM_H
#define GEM_H

#include "game/item/item.h"

enum class GemType
{
    멸화, 홍염
};

class Gem : public Item
{
public:
    Gem(GemType gemType);

    GemType gemType() const;

    int gemLevel() const;
    void setGemLevel(int newGemLevel);

private:
    GemType m_gemType;
    int m_gemLevel;
};

#endif // GEM_H
