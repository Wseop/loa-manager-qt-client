#include "font_manager.h"

FontManager *FontManager::mpFontManager = nullptr;

FontManager::FontManager()
{
    loadFont();
}

FontManager::~FontManager()
{

}

void FontManager::loadFont()
{
    // load font resources
    QFontDatabase::addApplicationFont(":/font/NanumSquareNeo-aLt.ttf");
    QFontDatabase::addApplicationFont(":/font/NanumSquareNeo-bRg.ttf");
    QFontDatabase::addApplicationFont(":/font/NanumSquareNeo-cBd.ttf");
    QFontDatabase::addApplicationFont(":/font/NanumSquareNeo-dEb.ttf");
    QFontDatabase::addApplicationFont(":/font/NanumSquareNeo-eHv.ttf");

    // initialize fontfamily list
    for (int i = 0; i < static_cast<int>(FontFamily::FontFamilySize); i++)
        mFontFamilies.append(QFontDatabase::applicationFontFamilies(i).at(0));
}

FontManager *FontManager::getInstance()
{
    if (mpFontManager == nullptr)
        mpFontManager = new FontManager();

    return mpFontManager;
}

void FontManager::destroyInstance()
{
    if (mpFontManager == nullptr)
        return;

    delete mpFontManager;
    mpFontManager = nullptr;
}

QFont FontManager::getFont(FontFamily fontFamily, int fontSize)
{
    int index = static_cast<int>(fontFamily);
    return QFont(mFontFamilies[index], fontSize);
}
