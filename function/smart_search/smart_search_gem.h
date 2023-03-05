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
    void initializeUI();
    void updateUI(const class Gem gem, Price price);
    void clearUI();

    void parseSearchResult(class QNetworkReply* pReply);

private:
    Ui::SmartSearchGem *ui;

    QList<QStringList> m_searchList;

    QList<QWidget*> m_gemWidgets;
    QList<QWidget*> m_widgets;
};

#endif // SMART_SEARCH_GEM_H
