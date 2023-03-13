#ifndef CARD_H
#define CARD_H

#include <QString>
#include <QList>

class Card
{
public:
    Card();

    QList<QPair<QString, int> > cardSets() const;
    void addCardSet(const QString &cardSet);

private:
    QList<QPair<QString, int>> mCardSets;
};

#endif // CARD_H
