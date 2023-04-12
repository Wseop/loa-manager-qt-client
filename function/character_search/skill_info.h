#ifndef SKILL_INFO_H
#define SKILL_INFO_H

#include "game/skill/skill.h"
#include "game/item/rune.h"

#include <QWidget>

namespace Ui {
class SkillInfo;
}

class SkillInfo : public QWidget
{
    Q_OBJECT

public:
    SkillInfo(const class Skill *pSkill);
    ~SkillInfo();

private:
    void initializeSkillLayout1(const QString &iconPath, int skillLevel);
    void addSkillIcon(const QString &iconPath);
    void addSkillLevelLabel(int skillLevel);

    void initializeSkillLayout2(const QString &skillName, bool bCounter);
    void addSkillNameLabel(const QString &skillName);
    void addCounterLabel(bool bCounter);

    void initializeTripodLayout(const QList<Tripod> &tripods);
    void addTripodTitle(const QString &titleText);
    void addTripodInfo(const Tripod &tripod);

    void initializeRuneLayout(const Rune *pRune);
    void addRuneIcon(const QString &iconPath, ItemGrade itemGrade);
    void addRuneNameLabel(const QString &runeName, ItemGrade itemGrade);

private:
    Ui::SkillInfo *ui;
};

#endif // SKILL_INFO_H
