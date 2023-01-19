#ifndef CLASS_H
#define CLASS_H

#include <QString>

enum class Class
{
    버서커,
    디스트로이어,
    워로드,
    홀리나이트,
    아르카나,
    서머너,
    바드,
    소서리스,
    배틀마스터,
    인파이터,
    기공사,
    창술사,
    스트라이커,
    블레이드,
    데모닉,
    리퍼,
    호크아이,
    데빌헌터,
    블래스터,
    스카우터,
    건슬링어,
    도화가,
    기상술사,
    슬레이어,
    Size
};

static inline QString classToStr(Class cls)
{
    switch (cls)
    {
    case Class::버서커:
        return "버서커";
    case Class::디스트로이어:
        return "디스트로이어";
    case Class::워로드:
        return "워로드";
    case Class::홀리나이트:
        return "홀리나이트";
    case Class::아르카나:
        return "아르카나";
    case Class::서머너:
        return "서머너";
    case Class::바드:
        return "바드";
    case Class::소서리스:
        return "소서리스";
    case Class::배틀마스터:
        return "배틀마스터";
    case Class::인파이터:
        return "인파이터";
    case Class::기공사:
        return "기공사";
    case Class::창술사:
        return "창술사";
    case Class::스트라이커:
        return "스트라이커";
    case Class::블레이드:
        return "블레이드";
    case Class::데모닉:
        return "데모닉";
    case Class::리퍼:
        return "리퍼";
    case Class::호크아이:
        return "호크아이";
    case Class::데빌헌터:
        return "데빌헌터";
    case Class::블래스터:
        return "블래스터";
    case Class::스카우터:
        return "스카우터";
    case Class::건슬링어:
        return "건슬링어";
    case Class::도화가:
        return "도화가";
    case Class::기상술사:
        return "기상술사";
    case Class::슬레이어:
        return "슬레이어";
    default:
        return "";
    }
}

static inline Class strToClass(QString cls)
{
    if (cls == "버서커")
        return Class::버서커;
    else if (cls == "디스트로이어")
        return Class::디스트로이어;
    else if (cls == "워로드")
        return Class::워로드;
    else if (cls == "홀리나이트")
        return Class::홀리나이트;
    else if (cls == "아르카나")
        return Class::아르카나;
    else if (cls == "서머너")
        return Class::서머너;
    else if (cls == "바드")
        return Class::바드;
    else if (cls == "소서리스")
        return Class::소서리스;
    else if (cls == "배틀마스터")
        return Class::배틀마스터;
    else if (cls == "인파이터")
        return Class::인파이터;
    else if (cls == "기공사")
        return Class::기공사;
    else if (cls == "창술사")
        return Class::창술사;
    else if (cls == "스트라이커")
        return Class::스트라이커;
    else if (cls == "블레이드")
        return Class::블레이드;
    else if (cls == "데모닉")
        return Class::데모닉;
    else if (cls == "리퍼")
        return Class::리퍼;
    else if (cls == "호크아이")
        return Class::호크아이;
    else if (cls == "데빌헌터")
        return Class::데빌헌터;
    else if (cls == "블래스터")
        return Class::블래스터;
    else if (cls == "스카우터")
        return Class::스카우터;
    else if (cls == "건슬링어")
        return Class::건슬링어;
    else if (cls == "도화가")
        return Class::도화가;
    else if (cls == "기상술사")
        return Class::기상술사;
    else if (cls == "슬레이어")
        return Class::슬레이어;
    else
        return Class::Size;
}

static inline bool isSupporter(Class cls)
{
    if (cls == Class::바드 || cls == Class::홀리나이트 || cls == Class::도화가)
        return true;
    else
        return false;
}

#endif // CLASS_H
