#ifndef COLLECTIBLE_TYPE_H
#define COLLECTIBLE_TYPE_H

#include <QString>

enum class CollectibleType
{
    Mokoko,
    Island,
    GreatPicture,
    GiantHeart,
    Ignea,
    Voyage,
    WorldTree,
    Orpeus,
    Orgel,
    Size
};

static inline QString collectibleToStr(CollectibleType type)
{
    switch (type)
    {
    case CollectibleType::Mokoko:
        return "모코코 씨앗";
    case CollectibleType::Island:
        return "섬의 마음";
    case CollectibleType::GreatPicture:
        return "위대한 미술품";
    case CollectibleType::GiantHeart:
        return "거인의 심장";
    case CollectibleType::Ignea:
        return "이그네아의 징표";
    case CollectibleType::Voyage:
        return "항해 모험물";
    case CollectibleType::WorldTree:
        return "세계수의 잎";
    case CollectibleType::Orpeus:
        return "오르페우스의 별";
    case CollectibleType::Orgel:
        return "기억의 오르골";
    default:
        return "";
    }
}

static inline CollectibleType strToCollectibleType(QString type)
{
    if (type == "모코코 씨앗")
        return CollectibleType::Mokoko;
    else if (type == "섬의 마음")
        return CollectibleType::Island;
    else if (type == "위대한 미술품")
        return CollectibleType::GreatPicture;
    else if (type == "거인의 심장")
        return CollectibleType::GiantHeart;
    else if (type == "이그네아의 징표")
        return CollectibleType::Ignea;
    else if (type == "항해 모험물")
        return CollectibleType::Voyage;
    else if (type == "세계수의 잎")
        return CollectibleType::WorldTree;
    else if (type == "오르페우스의 별")
        return CollectibleType::Orpeus;
    else if (type == "기억의 오르골")
        return CollectibleType::Orgel;
    else
        return CollectibleType::Size;
}

#endif // COLLECTIBLE_TYPE_H
