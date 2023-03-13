#ifndef TRIPOD_H
#define TRIPOD_H

#include <QString>

class Tripod
{
public:
    Tripod();

    QString tripodName() const;
    void setTripodName(const QString &newTripodName);

    int tier() const;
    void setTier(int newTier);

    int tripodCode() const;
    void setTripodCode(int newTripodCode);

    int maxLevel() const;
    void setMaxLevel(int newMaxLevel);

    QString iconPath() const;
    void setIconPath(const QString &newIconPath);

    int tripodLevel() const;
    void setTripodLevel(int newTripodLevel);

private:
    QString mTripodName;
    int mTier;
    int mTripodCode;
    int mMaxLevel;
    QString mIconPath;

    int mTripodLevel;
};

#endif // TRIPOD_H
