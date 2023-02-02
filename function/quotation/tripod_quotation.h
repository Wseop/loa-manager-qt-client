#ifndef TRIPOD_QUOTATION_H
#define TRIPOD_QUOTATION_H

#include <QWidget>

struct SkillInfo
{
    QString name;
    int auctionCode;
    QString iconPath;
};

struct TripodInfo
{
    QString name;
    int auctionCode;
    int iconIndex;
};

namespace Ui {
class TripodQuotation;
}

class TripodQuotation : public QWidget
{
    Q_OBJECT

private:
    TripodQuotation();
    ~TripodQuotation();

    void loadSkillData();
    void setFonts();
    void setAlignments();
    void initConnects();
    void clear();
    void addSkillInfoWidgets(QString cls);

public:
    static TripodQuotation* getInstance();
    static void destroyInstance();

private:
    Ui::TripodQuotation *ui;

    static TripodQuotation* m_pInstance;

    class ClassSelector* m_pClassSelector;
    QMap<QString, QList<SkillInfo>> m_classToSkills;
    QMap<QString, QList<TripodInfo>> m_skillToTripods;
    QList<class SkillInfoWidget*> m_skillInfoWidgets;
};

#endif // TRIPOD_QUOTATION_H
