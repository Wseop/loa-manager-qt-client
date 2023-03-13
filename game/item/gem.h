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

    QString skillName() const;
    void setSkillName(const QString &newSkillName);

private:
    GemType mGemType;
    int mGemLevel;
    QString mSkillName;
};

#endif // GEM_H
