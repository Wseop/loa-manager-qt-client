#ifndef LOSTARK_API_H
#define LOSTARK_API_H

enum class LostarkApi
{
    Sibling,
    Profile,
    Equipment,
    Skill,
    Engrave,
    Card,
    Gem,
    Collectible,
    Auction,
    Market,
    Size
};

enum class CategoryCode
{
    EngraveBook = 40000,
    AbilityStone = 30000,
    Reforge = 50000,
    Tripod = 170300,
    Accessory = 200000,
    Necklace = 200010,
    Earring = 200020,
    Ring = 200030,
    Bracelet = 200040,
    Gem = 210000
};

enum class AbilityCode
{
    치명 = 15,
    특화,
    제압,
    신속,
    인내,
    숙련
};

enum class SearchType
{
    Auction,
    Market
};

#endif // LOSTARK_API_H
