#ifndef SMART_SEARCH_GEM_H
#define SMART_SEARCH_GEM_H

#include "function/smart_search/smart_search_menu.h"

#include <QWidget>

namespace Ui {
class SmartSearchGem;
}

class SmartSearchGem : public QWidget, public SmartSearchMenu
{
    Q_OBJECT

public:
    SmartSearchGem(QLayout* pLayout);
    ~SmartSearchGem();

    void refresh() override;

private:
    void initUI();
    void updateUI(const class Gem gem, const int price);
    void clearUI();

private:
    Ui::SmartSearchGem *ui;

    const int LABEL_WIDTH[3] = {50, 200, 150};

    QList<QStringList> m_searchList;
    QList<class QNetworkAccessManager*> m_iconLoaders;

    QList<QWidget*> m_gemWidgets;
    QList<QWidget*> m_widgets;
};

#endif // SMART_SEARCH_GEM_H
