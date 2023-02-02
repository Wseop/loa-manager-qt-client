#ifndef SKILL_INFO_WIDGET_H
#define SKILL_INFO_WIDGET_H

#include "function/quotation/tripod_quotation.h"
#include <QWidget>

namespace Ui {
class SkillInfoWidget;
}

class SkillInfoWidget : public QWidget
{
    Q_OBJECT

public:
    SkillInfoWidget(QWidget* pParent, const SkillInfo& skill, const QList<TripodInfo>& tripods);
    ~SkillInfoWidget();

private:
    void setFonts();
    void setAlignments();
    void addSkill(const SkillInfo& skill);
    void addTripods(const QList<TripodInfo>& tripods);

private:
    Ui::SkillInfoWidget *ui;

    const int ICON_WIDTH = 50;
    const int ICON_HEIGHT = 50;
    const int LABEL_WIDTH = 125;
    const int LABEL_HEIGHT = 25;

    QList<class QLabel*> m_labels;
    QList<class QBoxLayout*> m_layouts;
};

#endif // SKILL_INFO_WIDGET_H
