#include "search_option.h"

/*
 * SearchOption
 */
SearchOption::SearchOption() :
    mItemGrade(ItemGrade::size),
    mPageAll(false)
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
        query += QString("&itemGrade=%1").arg(itemGradeToQString(mItemGrade));
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

QString MarketSearchOption::getQuery()
{
    QString query = SearchOption::getQuery();

    query += QString("&categoryCode=%1").arg(static_cast<int>(mMarketCategory));

    if (mClassName != "") {
        query += QString("&className=%1").arg(mClassName);
    }

    return query;
}

void MarketSearchOption::setMarketCategory(MarketCategory newMarketCategory)
{
    mMarketCategory = newMarketCategory;
}

void MarketSearchOption::setClassName(const QString &newClassName)
{
    mClassName = newClassName;
}
