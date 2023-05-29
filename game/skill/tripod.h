#ifndef TRIPOD_H
#define TRIPOD_H

#include <QString>

struct Tripod {
    QString tripodName;
    int tripodCode = -1;
    int iconIndex = -1;
    int maxLevel = -1;
    int tier = -1;
    QString iconPath;

    int tripodLevel = -1;
    bool isSelected = false;
};

#endif // TRIPOD_H
