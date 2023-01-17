#include "card_widget.h"
#include "ui_card_widget.h"
#include "ui/widget_manager.h"
#include "game_data/character/card/card.h"
#include <QLabel>

CardWidget::CardWidget(QWidget* pParent, const QList<Card*>& cards) :
    QWidget(pParent),
    ui(new Ui::CardWidget),
    m_cards(cards)
{
    ui->setupUi(this);
    ui->vLayoutCard->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    addCardLabels();
}

CardWidget::~CardWidget()
{
    for (QLabel* pLabel : m_labels)
        delete pLabel;
    delete ui;
}

void CardWidget::addCardLabels()
{
    for (const Card* pCard : m_cards)
    {
        // 최종완성단계 세트효과만 추가
        const QString& cardEffectName = pCard->getEffectNames().back();
        QString cardEffectText;

        // 카드효과 (+ 각성합계) 문자열 생성
        if (cardEffectName.contains("각성합계"))
        {
            // 각성합계 제외, 카드효과만 추출
            int nameSize = cardEffectName.indexOf("(") - 1;
            cardEffectText = cardEffectName.sliced(0, nameSize);
            // 각성합계 수치 추가
            nameSize += 2;
            int awaken = cardEffectName.sliced(nameSize, cardEffectName.indexOf("각성합계") - nameSize).toInt();
            cardEffectText += QString(" %1").arg(awaken);
        }
        else
        {
            cardEffectText = cardEffectName;
        }

        QLabel* pCardLabel = WidgetManager::createLabel(cardEffectText, LABEL_WIDTH, LABEL_HEIGHT, 10, this);
        m_labels.append(pCardLabel);
        ui->vLayoutCard->addWidget(pCardLabel);
    }
}
