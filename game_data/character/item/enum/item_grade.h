#ifndef ITEM_GRADE_H
#define ITEM_GRADE_H

#include <QString>

enum class ItemGrade
{
    일반,
    고급,
    희귀,
    영웅,
    전설,
    유물,
    고대,
    에스더,
    Size
};

static inline QString itemGradeToStr(ItemGrade grade)
{
    switch (grade)
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
    default:
        return "";
    }
}

static inline ItemGrade strToItemGrade(QString grade)
{
    if ("일반" == grade)
        return ItemGrade::일반;
    else if ("고급" == grade)
        return ItemGrade::고급;
    else if ("희귀" == grade)
        return ItemGrade::희귀;
    else if ("영웅" == grade)
        return ItemGrade::영웅;
    else if ("전설" == grade)
        return ItemGrade::전설;
    else if ("유물" == grade)
        return ItemGrade::유물;
    else if ("고대" == grade)
        return ItemGrade::고대;
    else if ("에스더" == grade)
        return ItemGrade::에스더;
    else
        return ItemGrade::일반;
}

static inline QString colorCode(ItemGrade grade)
{
    switch (grade)
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

#endif // ITEM_GRADE_H
