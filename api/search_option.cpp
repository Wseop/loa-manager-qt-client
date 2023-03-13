#include "search_option.h"

SearchOption::SearchOption(SearchType type)
{
    setSearchType(type);
}

SearchOption::SearchOption(const SearchOption &searchOption) :
    mSearchOption(searchOption.mSearchOption),
    mSearchType(searchOption.mSearchType),
    mSkillOptions(searchOption.mSkillOptions),
    mEtcOptions(searchOption.mEtcOptions)
{

}

void SearchOption::setSearchType(SearchType type)
{
    mSearchType = type;

    if (mSearchType == SearchType::Auction)
        mSearchOption.insert("Sort", "BUY_PRICE");
    else
        mSearchOption.insert("Sort", "CURRENT_MIN_PRICE");
}

void SearchOption::setCategoryCode(CategoryCode category)
{
    mSearchOption.insert("CategoryCode", static_cast<int>(category));
}

void SearchOption::setItemTier(int tier)
{
    mSearchOption.insert("ItemTier", tier);
}

void SearchOption::setItemGrade(ItemGrade itemGrade)
{
    mSearchOption.insert("ItemGrade", itemGradeToQString(itemGrade));
}

void SearchOption::setItemName(QString itemName)
{
    mSearchOption.insert("ItemName", itemName);
}

void SearchOption::setPageNo(int pageNo)
{
    mSearchOption.insert("PageNo", pageNo);
}

void SearchOption::setSortCondition(QString sortCondition)
{
    if (sortCondition == "ASC" || sortCondition == "DESC")
        mSearchOption.insert("SortCondition", sortCondition);
}

void SearchOption::setQuality(int quality)
{
    if (mSearchType == SearchType::Auction)
        mSearchOption.insert("ItemGradeQuality", quality);
}

void SearchOption::setSkillOption(int firstOption, int secondOption, int minValue, int maxValue)
{
    if (mSearchType == SearchType::Market)
        return;

    QJsonObject skillOption;
    skillOption.insert("FirstOption", firstOption);
    skillOption.insert("SecondOption", secondOption);
    if (minValue != -1)
        skillOption.insert("MinValue", minValue);
    if (maxValue != -1)
        skillOption.insert("MaxValue", maxValue);
    mSkillOptions.append(skillOption);
}

void SearchOption::setEtcOption(EtcOptionCode firstOption, int secondOption, int minValue, int maxValue)
{
    if (mSearchType == SearchType::Market)
        return;

    QJsonObject etcOption;
    etcOption.insert("FirstOption", static_cast<int>(firstOption));
    etcOption.insert("SecondOption", secondOption);
    if (minValue != -1)
        etcOption.insert("MinValue", minValue);
    if (maxValue != -1)
        etcOption.insert("MaxValue", maxValue);
    mEtcOptions.append(etcOption);
}

QJsonObject SearchOption::toJsonObject()
{
    if (mSearchType == SearchType::Auction)
    {
        if (mSkillOptions.size() > 0)
            mSearchOption.insert("SkillOptions", mSkillOptions);
        if (mEtcOptions.size() > 0)
            mSearchOption.insert("EtcOptions", mEtcOptions);
    }

    return mSearchOption;
}
