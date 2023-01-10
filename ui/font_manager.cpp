#include "font_manager.h"

FontManager* FontManager::m_pFontManager = nullptr;

FontManager::FontManager()
{
    loadFont();
}

FontManager::~FontManager()
{
    destroyInstance();
}

void FontManager::loadFont()
{
    // load font resources
    QFontDatabase::addApplicationFont(":/font/font/NanumSquareNeo-aLt.ttf");
    QFontDatabase::addApplicationFont(":/font/font/NanumSquareNeo-bRg.ttf");
    QFontDatabase::addApplicationFont(":/font/font/NanumSquareNeo-cBd.ttf");
    QFontDatabase::addApplicationFont(":/font/font/NanumSquareNeo-dEb.ttf");
    QFontDatabase::addApplicationFont(":/font/font/NanumSquareNeo-eHv.ttf");

    // initialize fontfamily list
    for (int i = 0; i < static_cast<int>(FontFamily::FontFamilySize); i++)
        m_fontFamilies.append(QFontDatabase::applicationFontFamilies(i).at(0));
}

FontManager* FontManager::getInstance()
{
    if (m_pFontManager == nullptr)
        m_pFontManager = new FontManager();

    return m_pFontManager;
}

void FontManager::destroyInstance()
{
    if (m_pFontManager == nullptr)
        return;

    delete m_pFontManager;
}

QFont FontManager::getFont(FontFamily fontFamily, int fontSize)
{
    int index = static_cast<int>(fontFamily);
    return QFont(m_fontFamilies[index], fontSize);
}
