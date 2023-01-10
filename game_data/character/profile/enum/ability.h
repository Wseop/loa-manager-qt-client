#ifndef ABILITY_H
#define ABILITY_H

#include <QString>

enum class Ability
{
    치명,
    특화,
    신속,
    제압,
    인내,
    숙련,
    Size
};

static inline QString abilityToStr(Ability ability)
{
    switch (ability)
    {
    case Ability::치명:
        return "치명";
    case Ability::특화:
        return "특화";
    case Ability::신속:
        return "신속";
    case Ability::제압:
        return "제압";
    case Ability::인내:
        return "인내";
    case Ability::숙련:
        return "숙련";
    default:
        return "";
    }
}

static inline Ability strToAbility(QString ability)
{
    if ("치명" == ability)
        return Ability::치명;
    else if ("특화" == ability)
        return Ability::특화;
    else if ("신속" == ability)
        return Ability::신속;
    else if ("제압" == ability)
        return Ability::제압;
    else if ("인내" == ability)
        return Ability::인내;
    else if ("숙련" == ability)
        return Ability::숙련;
    else
        return Ability::Size;
}

#endif // ABILITY_H
