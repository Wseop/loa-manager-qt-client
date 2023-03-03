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

private:
    QString m_tripodName;
    int m_tier;
    int m_tripodCode;
    int m_maxLevel;
    QString m_iconPath;
};

#endif // TRIPOD_H
