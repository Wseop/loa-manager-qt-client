#ifndef ITEM_GRADE_H
#define ITEM_GRADE_H

#include <QString>

enum class ItemGrade
{
    일반, 고급, 희귀, 영웅, 전설, 유물, 고대, 에스더
};

static inline QString itemGradeToQString(ItemGrade itemGrade)
{
    switch (itemGrade)
    {
    case ItemGrade::일반:
        return "일반";
    case ItemGrade::고급:
        return "고급";
    case ItemGrade::희귀:
        return "희귀";
    case ItemGrade::영웅:
        return "영웅";
    case ItemGrade::전설:
        return "전설";
    case ItemGrade::유물:
        return "유물";
    case ItemGrade::고대:
        return "고대";
    case ItemGrade::에스더:
        return "에스더";
    }
}

static inline ItemGrade qStringToItemGrade(const QString& itemGrade)
{
    if (itemGrade == "일반")
        return ItemGrade::일반;
    else if (itemGrade == "고급")
        return ItemGrade::고급;
    else if (itemGrade == "희귀")
        return ItemGrade::희귀;
    else if (itemGrade == "영웅")
        return ItemGrade::영웅;
    else if (itemGrade == "전설")
        return ItemGrade::전설;
    else if (itemGrade == "유물")
        return ItemGrade::유물;
    else if (itemGrade == "고대")
        return ItemGrade::고대;
    else if (itemGrade == "에스더")
        return ItemGrade::에스더;
}

static inline QString itemGradeToTextColor(ItemGrade itemGrade)
{
    switch (itemGrade)
    {
    case ItemGrade::일반:
        return "#000000";
    case ItemGrade::고급:
        return "#00B700";
    case ItemGrade::희귀:
        return "#00B0FA";
    case ItemGrade::영웅:
        return "#CE43FC";
    case ItemGrade::전설:
        return "#F99200";
    case ItemGrade::유물:
        return "#FA5D00";
    case ItemGrade::고대:
        return "#D7AC87";
    case ItemGrade::에스더:
        return "#36D6CC";
    default:
        return "#000000";
    }
}

static inline QString itemGradeToBGColor(ItemGrade itemGrade)
{
    switch (itemGrade)
    {
    case ItemGrade::일반:
        return "#2C2A2A";
    case ItemGrade::고급:
        return "#2C4310";
    case ItemGrade::희귀:
        return "#113855";
    case ItemGrade::영웅:
        return "#361046";
    case ItemGrade::전설:
        return "#432803";
    case ItemGrade::유물:
        return "#401E08";
    case ItemGrade::고대:
        return "#493F2E";
    case ItemGrade::에스더:
        return "#0F3937";
    default:
        return "#000000";
    }
}

#endif // ITEM_GRADE_H
