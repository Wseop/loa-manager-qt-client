#ifndef ABILITYSTONE_QUOTATION_H
#define ABILITYSTONE_QUOTATION_H

#include <QWidget>

namespace Ui {
class AbilityStoneQuotation;
}

class AbilityStoneQuotation : public QWidget
{
    Q_OBJECT

private:
    AbilityStoneQuotation();
    ~AbilityStoneQuotation();

    void initEngraveSelectors();

    void search();
    void sendApiRequest(QString engrave1, QString engrave2);
    void addResult(QString engrave1, QString engrave2, int price);

public:
    static AbilityStoneQuotation* getInstance();
    static void destroyInstance();

private:
    Ui::AbilityStoneQuotation *ui;

    static AbilityStoneQuotation* m_pInstance;

    QList<class EngraveSelector*> m_engraveSelectors;
    QList<class QPushButton*> m_engraveSelectButtons;

    QList<QWidget*> m_widgets;
    QList<QLayout*> m_layouts;
};

#endif // ABILITYSTONE_QUOTATION_H
