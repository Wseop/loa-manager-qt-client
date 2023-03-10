#ifndef ITEM_SET_H
#define ITEM_SET_H

#include <QString>

enum class ItemSet
{
    지배, 배신, 파괴, 매혹, 사멸, 악몽, 환각, 구원, 갈망, size
};

static inline QString itemSetToQString(ItemSet itemSet)
{
    switch (itemSet)
    {
    case ItemSet::지배:
        return "지배";
    case ItemSet::배신:
        return "배신";
    case ItemSet::파괴:
        return "파괴";
    case ItemSet::매혹:
        return "매혹";
    case ItemSet::사멸:
        return "사멸";
    case ItemSet::악몽:
        return "악몽";
    case ItemSet::환각:
        return "환각";
    case ItemSet::구원:
        return "구원";
    case ItemSet::갈망:
        return "갈망";
    default:
        return "";
    }
}

static inline ItemSet qStringToItemSet(const QString &itemSet)
{
    if (itemSet == "지배")
        return ItemSet::지배;
    else if (itemSet == "배신")
        return ItemSet::배신;
    else if (itemSet == "파괴")
        return ItemSet::파괴;
    else if (itemSet == "매혹")
        return ItemSet::매혹;
    else if (itemSet == "사멸")
        return ItemSet::사멸;
    else if (itemSet == "악몽")
        return ItemSet::악몽;
    else if (itemSet == "환각")
        return ItemSet::환각;
    else if (itemSet == "구원")
        return ItemSet::구원;
    else if (itemSet == "갈망")
        return ItemSet::갈망;
    else
        return ItemSet::size;
}

#endif // ITEM_SET_H
