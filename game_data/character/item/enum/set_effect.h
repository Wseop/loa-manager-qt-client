#ifndef SET_EFFECT_H
#define SET_EFFECT_H

#include <QString>

enum class SetEffect
{
    악몽,
    구원,
    사멸,
    지배,
    환각,
    갈망,
    배신,
    매혹,
    파괴,
    에스더,
    Size
};

static inline QString setEffectToStr(SetEffect setEffect)
{
    switch (setEffect)
    {
    case SetEffect::구원:
        return "구원";
    case SetEffect::지배:
        return "지배";
    case SetEffect::악몽:
        return "악몽";
    case SetEffect::사멸:
        return "사멸";
    case SetEffect::환각:
        return "환각";
    case SetEffect::갈망:
        return "갈망";
    case SetEffect::배신:
        return "배신";
    case SetEffect::매혹:
        return "매혹";
    case SetEffect::파괴:
        return "파괴";
    case SetEffect::에스더:
        return "에스더";
    default:
        return "";
    }
}

static inline SetEffect strToSetEffect(QString setEffect)
{
    if ("구원" == setEffect)
        return SetEffect::구원;
    else if ("지배" == setEffect)
        return SetEffect::지배;
    else if ("악몽" == setEffect)
        return SetEffect::악몽;
    else if ("사멸" == setEffect)
        return SetEffect::사멸;
    else if ("환각" == setEffect)
        return SetEffect::환각;
    else if ("갈망" == setEffect)
        return SetEffect::갈망;
    else if ("배신" == setEffect)
        return SetEffect::배신;
    else if ("매혹" == setEffect)
        return SetEffect::매혹;
    else if ("파괴" == setEffect)
        return SetEffect::파괴;
    else if ("에스더" == setEffect)
        return SetEffect::에스더;
    else
        return SetEffect::Size;
}

#endif // SET_EFFECT_H
