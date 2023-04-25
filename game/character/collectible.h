#ifndef COLLECTIBLE_H
#define COLLECTIBLE_H

#include <QString>

class Collectible
{
public:
    Collectible(QString type, int point, int maxPoint);

    QString getType() const;
    QString getIconPath() const;
    int getPoint() const;
    int getMaxPoint() const;

private:
    QString mType;
    QString mIconPath;
    int mPoint;
    int mMaxPoint;
};

#endif // COLLECTIBLE_H
