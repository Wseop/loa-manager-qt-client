#ifndef COLLECTIBLE_H
#define COLLECTIBLE_H

#include "game_data/character/collectible/enum/collectible_type.h"
#include <QObject>

class Collectible : public QObject
{
    Q_OBJECT

public:
    Collectible();

    const CollectibleType& getType() const;
    const int& getMaxPoint() const;
    const int& getPoint() const;

    void setType(const CollectibleType& type);
    void setMaxPoint(const int& maxPoint);
    void setPoint(const int& point);

private:
    CollectibleType m_type;
    int m_maxPoint;
    int m_point;
};

#endif // COLLECTIBLE_H
