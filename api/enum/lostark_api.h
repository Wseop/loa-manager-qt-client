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
    AbilityStone = 30000,
    Reforge = 50000,
    Tripod = 170300,
    Gem = 210000
};

enum class SearchType
{
    Auction,
    Market
};

#endif // LOSTARK_API_H
