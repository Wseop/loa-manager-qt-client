#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include <QObject>

#include <QFont>
#include <QFontDatabase>

enum class FontFamily
{
    NanumSquareNeoLight,
    NanumSquareNeoRegular,
    NanumSquareNeoBold,
    NanumSquareNeoExtraBold,
    NanumSquareNeoHeavy,
    FontFamilySize
};

class FontManager
{
private:
    FontManager();
    ~FontManager();

    void loadFont();

public:
    static FontManager *getInstance();
    static void destroyInstance();

    QFont getFont(FontFamily fontFamily, int fontSize);

private:
    static FontManager *mpFontManager;

    QStringList mFontFamilies;
};

#endif // FONTMANAGER_H
