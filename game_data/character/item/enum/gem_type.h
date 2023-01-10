#ifndef GEM_TYPE_H
#define GEM_TYPE_H

#include <QString>

enum class GemType
{
    멸화,
    홍염,
    Size
};

static inline QString gemTypeToStr(GemType type)
{
    switch (type)
    {
    case GemType::멸화:
        return "멸화";
    case GemType::홍염:
        return "홍염";
    default:
        return "";
    }
}

static inline GemType strToGemType(QString type)
{
    if ("멸화" == type)
        return GemType::멸화;
    else if ("홍염" == type)
        return GemType::홍염;
    else
        return GemType::Size;
}

#endif // GEM_TYPE_H
