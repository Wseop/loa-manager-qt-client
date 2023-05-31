#ifndef GEM_INFO_H
#define GEM_INFO_H

#include "game/item/item_grade.h"
#include "game/item/gem.h"

#include <QWidget>

namespace Ui {
class GemInfo;
}

class GemInfo : public QWidget
{
    Q_OBJECT

public:
    GemInfo(const Gem &gem);
    ~GemInfo();

private:
    void initializeLayout1(const QString &iconPath, const ItemGrade &itemGrade, int gemLevel);
    void addGemIcon(const QString &iconPath, ItemGrade itemGrade);
    void addGemLevelInfo(int gemLevel, ItemGrade itemGrade);

    void initializeLayout2(const QString &type, const ItemGrade &itemGrade, const QString &skillName);
    void addGemTypeInfo(const QString &type, const ItemGrade &itemGrade);
    void addSkillNameInfo(const QString &skillName);

private:
    Ui::GemInfo *ui;
};

#endif // GEM_INFO_H
