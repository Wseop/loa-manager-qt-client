#ifndef ABILITY_H
#define ABILITY_H

#include <QString>

enum class Ability
{
    치명, 특화, 신속, 제압, 인내, 숙련, size
};

static inline QString abilityToQString(Ability ability)
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

static inline Ability qStringToAbility(QString ability)
{
    if (ability == "치명")
        return Ability::치명;
    else if (ability == "특화")
        return Ability::특화;
    else if (ability == "신속")
        return Ability::신속;
    else if (ability == "제압")
        return Ability::제압;
    else if (ability == "인내")
        return Ability::인내;
    else if (ability == "숙련")
        return Ability::숙련;
    else
        return Ability::size;
}

#endif // ABILITY_H
