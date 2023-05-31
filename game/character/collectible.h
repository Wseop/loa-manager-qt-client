#ifndef COLLECTIBLE_H
#define COLLECTIBLE_H

#include <QString>

struct Collectible {
    QString type;
    int point = -1;
    int maxPoint = -1;
};

#endif // COLLECTIBLE_H
