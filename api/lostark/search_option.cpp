#include "search_option.h"

/*
 * SearchOption
 */
SearchOption::SearchOption() :
    mItemGrade(ItemGrade::size),
    mPageAll(false)
{

}

SearchOption::SearchOption(const SearchOption &other) :
    mItemGrade(other.mItemGrade),
    mItemName(other.mItemName),
    mPageAll(other.mPageAll)
{

}

SearchOption &SearchOption::operator=(const SearchOption &other)
{
    mItemGrade = other.mItemGrade;
    mItemName = other.mItemName;
    mPageAll = other.mPageAll;

    return *this;
}

SearchOption::~SearchOption()
{

}

QString SearchOption::getQuery()
{
    QString query = "?";

    if (mPageAll) {
        query += QString("pageAll=true");
    } else {
        query += QString("pageAll=false");
    }

    if (mItemGrade != ItemGrade::size) {
        query += QString("&itemGrade=%1").arg(itemGradeToStr(mItemGrade));
    }
    if (mItemName != "") {
        query += QString("&itemName=%1").arg(mItemName);
    }

    return query;
}

void SearchOption::setItemGrade(ItemGrade newItemGrade)
{
    mItemGrade = newItemGrade;
}

void SearchOption::setItemName(const QString &newItemName)
{
    mItemName = newItemName;
}

void SearchOption::setPageAll(bool newPageAll)
{
    mPageAll = newPageAll;
}

/*
 * AuctionSearchOption
 */
AuctionSearchOption::AuctionSearchOption(AuctionCategory auctionCategory) :
    mAuctionCategory(auctionCategory),
    mQuality(-1)
{

}

AuctionSearchOption::AuctionSearchOption(const AuctionSearchOption &other) :
    SearchOption(other),
    mAuctionCategory(other.mAuctionCategory),
    mQuality(other.mQuality),
    mSkillCodes(other.mSkillCodes),
    mTripodCodes(other.mTripodCodes),
    mAbilityCodes(other.mAbilityCodes),
    mEngraveCodes(other.mEngraveCodes)
{

}

AuctionSearchOption &AuctionSearchOption::operator=(const AuctionSearchOption &other)
{
    SearchOption::operator=(other);

    mAuctionCategory = other.mAuctionCategory;
    mQuality = other.mQuality;
    mSkillCodes = other.mSkillCodes;
    mTripodCodes = other.mTripodCodes;
    mAbilityCodes = other.mAbilityCodes;
    mEngraveCodes = other.mEngraveCodes;

    return *this;
}

AuctionSearchOption::~AuctionSearchOption()
{

}

QString AuctionSearchOption::getQuery()
{
    QString query = SearchOption::getQuery();

    query += QString("&categoryCode=%1").arg(static_cast<int>(mAuctionCategory));

    if (mQuality >= 0) {
        query += QString("&quality=%1").arg(mQuality);
    }

    for (int skillCode : mSkillCodes) {
        query += QString("&skillCodes=%1").arg(skillCode);
    }

    for (int tripodCode : mTripodCodes) {
        query += QString("&tripodCodes=%1").arg(tripodCode);
    }

    for (int abilityCode : mAbilityCodes) {
        query += QString("&abilityCodes=%1").arg(abilityCode);
    }

    for (int engraveCode : mEngraveCodes) {
        query += QString("&engraveCodes=%1").arg(engraveCode);
    }

    return query;
}

void AuctionSearchOption::setQuality(int quality)
{
    mQuality = quality;
}

void AuctionSearchOption::addSkillCode(int skillCode)
{
    mSkillCodes << skillCode;
}

void AuctionSearchOption::addTripodCode(int tripodCode)
{
    mTripodCodes << tripodCode;
}

void AuctionSearchOption::addAbilityCode(int abilityCode)
{
    mAbilityCodes << abilityCode;
}

void AuctionSearchOption::addEngraveCode(int engraveCode)
{
    mEngraveCodes << engraveCode;
}

/*
 * MarketSearchOption
 */

MarketSearchOption::MarketSearchOption(MarketCategory marketCategory) :
    mMarketCategory(marketCategory)
{

}

MarketSearchOption::MarketSearchOption(const MarketSearchOption &other) :
    SearchOption(other),
    mMarketCategory(other.mMarketCategory),
    mClassName(other.mClassName)
{

}

MarketSearchOption &MarketSearchOption::operator=(const MarketSearchOption &other)
{
    SearchOption::operator=(other);

    mMarketCategory = other.mMarketCategory;
    mClassName = other.mClassName;

    return *this;
}

MarketSearchOption::~MarketSearchOption()
{

}

QString MarketSearchOption::getQuery()
{
    QString query = SearchOption::getQuery();

    query += QString("&categoryCode=%1").arg(static_cast<int>(mMarketCategory));

    if (mClassName != "") {
        query += QString("&className=%1").arg(mClassName);
    }

    return query;
}

void MarketSearchOption::setClassName(const QString &newClassName)
{
    mClassName = newClassName;
}
