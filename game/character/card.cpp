#include "card.h"

Card::Card()
{

}

QList<QPair<QString, int> > Card::cardSets() const
{
    return mCardSets;
}

void Card::addCardSet(const QString &cardSet)
{
    int awakenIndex = cardSet.indexOf("(");
    QString cardSetName = cardSet;
    int awaken = 0;

    // 각성 단계가 포함된 문자열은 카드 세트명과 각성 단계를 분리하여 추가
    if (awakenIndex != -1)
    {
        cardSetName = cardSet.sliced(0, awakenIndex - 1);
        awaken = cardSet.sliced(awakenIndex + 1, cardSet.indexOf("각성합계") - (awakenIndex + 1)).toInt();
    }

    mCardSets.append({cardSetName, awaken});
}
