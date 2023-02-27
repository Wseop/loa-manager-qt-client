#ifndef SMART_SEARCH_H
#define SMART_SEARCH_H

#include <QWidget>

namespace Ui {
class SmartSearch;
}

class SmartSearch : public QWidget
{
    Q_OBJECT

private:
    SmartSearch();
    ~SmartSearch();

    void loadResource();
    void initMenu();

public:
    static SmartSearch* getInstace();
    static void destroyInstance();

private:
    Ui::SmartSearch *ui;

    static SmartSearch* m_pInstance;

    QStringList m_menuNames;
    QList<class SmartSearchMenu*> m_menuWidgets;
    SmartSearchMenu* m_pSelectedMenu;

    QList<QWidget*> m_widgets;
};

#endif // SMART_SEARCH_H
