#ifndef SEARCHOPTION_H
#define SEARCHOPTION_H

#include "api/lostark/lostark_api.h"
#include "game/item/item_grade.h"

#include <QJsonObject>
#include <QJsonArray>

class SearchOption
{
public:
    SearchOption(SearchType type);
    SearchOption(const SearchOption &searchOption);

    void setSearchType(SearchType type);

    // auction, market 공용
    void setCategoryCode(CategoryCode category);
    void setItemTier(int tier);
    void setItemGrade(ItemGrade itemGrade);
    void setItemName(QString itemName);
    void setPageNo(int pageNo);
    void setSortCondition(QString sortCondition);

    // auction 전용
    void setQuality(int quality);
    void setSkillOption(int firstOption, int secondOption, int minValue = -1, int maxValue = -1);
    void setEtcOption(EtcOptionCode firstOption, int secondOption, int minValue = -1, int maxValue = -1);

    QJsonObject toJsonObject();

private:
    QJsonObject mSearchOption;
    SearchType mSearchType;
    QJsonArray mSkillOptions;
    QJsonArray mEtcOptions;
};

#endif // SEARCHOPTION_H
