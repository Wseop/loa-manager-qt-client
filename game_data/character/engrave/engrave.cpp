#include "engrave.h"

Engrave::Engrave()
{

}

const QList<PairEngraveValue>& Engrave::getEngraves() const
{
    return m_engraves;
}

const QList<PairEngraveValue>& Engrave::getPenalties() const
{
    return m_penalties;
}

void Engrave::addEngrave(const QString& engrave, int value)
{
    for (PairEngraveValue& pairEngraveValue : m_engraves)
    {
        if (pairEngraveValue.first == engrave)
        {
            pairEngraveValue.second += value;
            return;
        }
    }

    // 리스트에 각인이 존재하지 않으면 새로 추가
    m_engraves.append(PairEngraveValue{engrave, value});
}

void Engrave::addPenalty(const QString& penalty, int value)
{
    for (PairEngraveValue& pairEngraveValue : m_penalties)
    {
        if (pairEngraveValue.first == penalty)
        {
            pairEngraveValue.second += value;
            return;
        }
    }

    // 리스트에 각인이 존재하지 않으면 새로 추가
    m_penalties.append(PairEngraveValue{penalty, value});
}
