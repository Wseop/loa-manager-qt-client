#ifndef SMART_SEARCH_H
#define SMART_SEARCH_H

#include "function/function_widget.h"

#include <QWidget>

namespace Ui {
class SmartSearch;
}

class SmartSearch : public QWidget, public FunctionWidget
{
    Q_OBJECT

private:
    SmartSearch();
    ~SmartSearch();

    void loadResource();
    void initializeMenu();

public:
    static SmartSearch *getInstance();
    static void destroyInstance();

public:
    void start() override;

private:
    Ui::SmartSearch *ui;

    static SmartSearch *m_pInstance;

    QStringList m_menuNames;
    QList<class SmartSearchMenu*> m_menuWidgets;
    SmartSearchMenu *m_pSelectedMenu;
    class QPushButton* m_pSelectedMenuButton;

    QList<QWidget*> m_widgets;
};

#endif // SMART_SEARCH_H
