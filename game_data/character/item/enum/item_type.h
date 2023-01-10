#ifndef ITEM_TYPE_H
#define ITEM_TYPE_H

#include <QString>

enum class ItemType
{
    무기,
    투구,
    상의,
    하의,
    장갑,
    어깨,
    목걸이,
    귀걸이,
    반지,
    어빌리티_스톤,
    팔찌,
    보석,
    Size
};

static inline QString itemTypeToStr(ItemType type)
{
    switch (type)
    {
    case ItemType::무기:
        return "무기";
    case ItemType::투구:
        return "투구";
    case ItemType::상의:
        return "상의";
    case ItemType::하의:
        return "하의";
    case ItemType::장갑:
        return "장갑";
    case ItemType::어깨:
        return "어깨";
    case ItemType::목걸이:
        return "목걸이";
    case ItemType::귀걸이:
        return "귀걸이";
    case ItemType::반지:
        return "반지";
    case ItemType::어빌리티_스톤:
        return "어빌리티 스톤";
    case ItemType::팔찌:
        return "팔찌";
    case ItemType::보석:
        return "보석";
    default:
        return "";
    }
}

static inline ItemType strToItemType(QString type)
{
    if ("무기" == type)
        return ItemType::무기;
    else if ("투구" == type)
        return ItemType::투구;
    else if ("상의" == type)
        return ItemType::상의;
    else if ("하의" == type)
        return ItemType::하의;
    else if ("장갑" == type)
        return ItemType::장갑;
    else if ("어깨" == type)
        return ItemType::어깨;
    else if ("목걸이" == type)
        return ItemType::목걸이;
    else if ("귀걸이" == type)
        return ItemType::귀걸이;
    else if ("반지" == type)
        return ItemType::반지;
    else if ("어빌리티 스톤" == type)
        return ItemType::어빌리티_스톤;
    else if ("팔찌" == type)
        return ItemType::팔찌;
    else if ("보석" == type)
        return ItemType::보석;
    else
        return ItemType::Size;
}

#endif // ITEM_TYPE_H
