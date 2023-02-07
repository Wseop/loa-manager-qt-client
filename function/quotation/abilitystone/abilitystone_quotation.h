#ifndef ABILITYSTONE_QUOTATION_H
#define ABILITYSTONE_QUOTATION_H

#include <QWidget>
#include <QJsonObject>

namespace Ui {
class AbilityStoneQuotation;
}

class AbilityStoneQuotation : public QWidget
{
    Q_OBJECT

private:
    AbilityStoneQuotation();
    ~AbilityStoneQuotation();

    void setAlignments();
    void addEngraveSelectors();
    void search();
    void sendRequest(QString engrave1, QString engrave2);
    QJsonObject buildAbilityStoneSearchOption(int engraveCode1, int engraveCode2);
    void addResult(QString engrave1, QString engrave2, int price);

public:
    static AbilityStoneQuotation* getInstance();
    static void destroyInstance();

private:
    Ui::AbilityStoneQuotation *ui;

    static AbilityStoneQuotation* m_pInstance;

    QList<class EngraveSelector*> m_engraveSelectors;
    QList<class QPushButton*> m_engraveSelectButtons;
    QSet<QString> m_selectedEngraves;
};

#endif // ABILITYSTONE_QUOTATION_H
