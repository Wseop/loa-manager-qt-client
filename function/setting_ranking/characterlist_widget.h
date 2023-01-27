#ifndef CHARACTERLIST_WIDGET_H
#define CHARACTERLIST_WIDGET_H

#include "function/setting_ranking/setting_ranking.h"
#include <QWidget>

namespace Ui {
class CharacterListWidget;
}

class CharacterListWidget : public QWidget
{
    Q_OBJECT

public:
    CharacterListWidget(QWidget* pParent, const QList<CharacterInfo>& characterInfos);
    ~CharacterListWidget();

private:
    void addCharacterButtons(const QList<CharacterInfo>& characterInfos);

private:
    Ui::CharacterListWidget *ui;

    const int MAX_COL = 5;
    QList<class QPushButton*> m_buttons;
};

#endif // CHARACTERLIST_WIDGET_H
