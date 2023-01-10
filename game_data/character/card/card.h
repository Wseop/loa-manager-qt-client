#ifndef CARD_H
#define CARD_H

#include <QList>
#include <QString>

class Card
{
public:
    Card();

    const QList<QString>& getEffectNames() const;
    const QList<QString>& getEffectDescs() const;

    void addCardEffect(const QString& name, const QString& desc);

private:
    QList<QString> m_effectNames;
    QList<QString> m_effectDescs;
};

#endif // CARD_H
