#ifndef TRIPOD_SEARCH_ITEM_H
#define TRIPOD_SEARCH_ITEM_H

#include "api/lostark/item_manager.h"

#include <QWidget>

class QLabel;
class QGroupBox;
class QVBoxLayout;

namespace Ui {
class TripodSearchItem;
}

class TripodSearchItem : public QWidget
{
    Q_OBJECT

public:
    TripodSearchItem(const AuctionItem &item, const QString &className);
    ~TripodSearchItem();

private:
    QVBoxLayout *createItemLayout(const QString &iconPath, const QString &itemName, const QString &itemGrade);
    QLabel *createItemIcon(const QString &iconPath);
    QLabel *createItemNameLabel(const QString &itemName, const QString &itemGrade);

    QVBoxLayout *createSkillLayout(const QString &skillName, const QString &className);
    QLabel *createSkillIcon(const QString &skillName, const QString &className);
    QLabel *createSkillNameLabel(const QString &skillName);

    QVBoxLayout *createTripodLayout(const QString &tripodName, const QString &skillName, const QString &className, int level);
    QLabel *createTripodIcon(const QString &tripodName, const QString &skillName, const QString &className);
    QLabel *createTripodNameLabel(const QString &tripodName, int level);

    QGroupBox *createBidPriceGroup(int nextBidPrice);

    QGroupBox *createBuyPriceGroup(int buyPrice);

private:
    Ui::TripodSearchItem *ui;
};

#endif // TRIPOD_SEARCH_ITEM_H
