#ifndef SEARCHOPTION_H
#define SEARCHOPTION_H

#include "api/lostark/search_category.h"
#include "game/item/item_grade.h"

#include <QObject>

class SearchOption : public QObject {
    Q_OBJECT

public:
    SearchOption();
    virtual ~SearchOption();

    virtual QString getQuery();

    void setItemGrade(ItemGrade newItemGrade);
    void setItemName(const QString &newItemName);
    void setPageAll(bool newPageAll);

protected:
    ItemGrade mItemGrade;
    QString mItemName;
    bool mPageAll;
};

class AuctionSearchOption : public SearchOption
{
public:
    AuctionSearchOption(AuctionCategory auctionCategory);
    ~AuctionSearchOption();

    QString getQuery() override;

    void setQuality(int quality);
    void addSkillCode(int skillCode);
    void addTripodCode(int tripodCode);
    void addAbilityCode(int abilityCode);
    void addEngraveCode(int engraveCode);

private:
    AuctionCategory mAuctionCategory;
    int mQuality;
    QList<int> mSkillCodes;
    QList<int> mTripodCodes;
    QList<int> mAbilityCodes;
    QList<int> mEngraveCodes;
};

class MarketSearchOption : public SearchOption
{
public:
    MarketSearchOption(MarketCategory marketCategory);
    ~MarketSearchOption();

    QString getQuery() override;

    void setMarketCategory(MarketCategory newMarketCategory);
    void setClassName(const QString &newClassName);

private:
    MarketCategory mMarketCategory;
    QString mClassName;
};

#endif // SEARCHOPTION_H
