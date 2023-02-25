#include "search_option.h"

SearchOption::SearchOption(SearchType type)
{
    setSearchType(type);
}

void SearchOption::setSearchType(SearchType type)
{
    m_searchType = type;

    if (m_searchType == SearchType::Auction)
        m_searchOption.insert("Sort", "BUY_PRICE");
    else
        m_searchOption.insert("Sort", "CURRENT_MIN_PRICE");
}

void SearchOption::setCategoryCode(CategoryCode category)
{
    m_searchOption.insert("CategoryCode", static_cast<int>(category));
}

void SearchOption::setItemTier(int tier)
{
    m_searchOption.insert("ItemTier", tier);
}

void SearchOption::setItemGrade(ItemGrade itemGrade)
{
    m_searchOption.insert("ItemGrade", itemGradeToStr(itemGrade));
}

void SearchOption::setItemName(QString itemName)
{
    m_searchOption.insert("ItemName", itemName);
}

void SearchOption::setPageNo(int pageNo)
{
    m_searchOption.insert("PageNo", pageNo);
}

void SearchOption::setSortCondition(QString sortCondition)
{
    if (sortCondition == "ASC" || sortCondition == "DESC")
        m_searchOption.insert("SortCondition", sortCondition);
}

void SearchOption::setQuality(int quality)
{
    if (m_searchType == SearchType::Auction)
        m_searchOption.insert("ItemGradeQuality", quality);
}

void SearchOption::setSkillOption(int firstOption, int secondOption, int minValue, int maxValue)
{
    if (m_searchType == SearchType::Market)
        return;

    QJsonObject skillOption;
    skillOption.insert("FirstOption", firstOption);
    skillOption.insert("SecondOption", secondOption);
    if (minValue != -1)
        skillOption.insert("MinValue", minValue);
    if (maxValue != -1)
        skillOption.insert("MaxValue", maxValue);
    m_skillOptions.append(skillOption);
}

void SearchOption::setEtcOption(int firstOption, int secondOption, int minValue, int maxValue)
{
    if (m_searchType == SearchType::Market)
        return;

    QJsonObject etcOption;
    etcOption.insert("FirstOption", firstOption);
    etcOption.insert("SecondOption", secondOption);
    if (minValue != -1)
        etcOption.insert("MinValue", minValue);
    if (maxValue != -1)
        etcOption.insert("MaxValue", maxValue);
    m_etcOptions.append(etcOption);
}

QJsonObject SearchOption::toJsonObject()
{
    if (m_searchType == SearchType::Auction)
    {
        if (m_skillOptions.size() > 0)
            m_searchOption.insert("SkillOptions", m_skillOptions);
        if (m_etcOptions.size() > 0)
            m_searchOption.insert("EtcOptions", m_etcOptions);
    }

    return m_searchOption;
}
