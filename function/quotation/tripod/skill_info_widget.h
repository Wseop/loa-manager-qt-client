#ifndef SKILL_INFO_WIDGET_H
#define SKILL_INFO_WIDGET_H

#include "function/quotation/tripod/tripod_quotation.h"

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

    void setPriceEmphasis(int price);

private:
    void initFont();
    void addSkill();
    void addTripods();
    void setTripodPrice(int tripodCode, class QLabel* pLabelPrice);

private:
    Ui::SkillInfoWidget *ui;

    const int LABEL_WIDTH = 125;
    const int LABEL_HEIGHT = 25;

    const SkillInfo& m_skillInfo;
    const QList<TripodInfo>& m_tripods;

    QList<QLabel*> m_labels;
    QList<QLabel*> m_priceLabels;
    QList<class QBoxLayout*> m_layouts;
};

#endif // SKILL_INFO_WIDGET_H
