#ifndef SMART_SEARCH_TRIPOD_H
#define SMART_SEARCH_TRIPOD_H

#include "function/smart_search/smart_search_menu.h"
#include "game/skill/skill.h"

#include <QWidget>

namespace Ui {
class SmartSearchTripod;
}

class SmartSearchTripod : public QWidget, public SmartSearchMenu
{
    Q_OBJECT

    const QString LABEL_STYLE_NORMAL = "QLabel { border: 1px solid black }";
    const QString LABEL_STYLE_EMPHASIS = "QLabel { border: 1px solid black; color: red }";

public:
    SmartSearchTripod(QLayout *pLayout);
    ~SmartSearchTripod();

    void refresh() override;

private:
    void initializeClassSelector();
    void initializePriceFilter();
    void initializeSearchButton();
    void initializeResultUI();

    void clearResult();
    void searchTripod(int skillCode, int tripodCode);
    void addSkillWidget(const Skill &skill, int row);
    void addTripodWidget(const Tripod &tripod, int skillCode, int row, int col);

private:
    Ui::SmartSearchTripod *ui;

    int mFilterPrice;

    class QComboBox* mpClassSelector;
    class QIntValidator* mpPriceValidator;

    // search result
    QHash<int, QHash<int, class QLabel*>> mTripodPriceLabels;
    QList<QWidget*> mResultWidgets;
    QList<QLayout*> mResultLayouts;

    QList<QWidget*> mWidgets;
    QList<QLayout*> mLayouts;
};

#endif // SMART_SEARCH_TRIPOD_H
