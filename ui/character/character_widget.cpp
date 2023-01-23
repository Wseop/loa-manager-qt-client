#include "character_widget.h"
#include "ui_character_widget.h"
#include "game_data/character/character.h"
#include "game_data/character/profile/profile.h"
#include "game_data/character/card/card.h"
#include "game_data/character/item/equip.h"
#include "game_data/character/item/accessory.h"
#include "game_data/character/item/abilitystone.h"
#include "game_data/character/item/bracelet.h"
#include "game_data/character/item/gem.h"
#include "game_data/character/engrave/engrave.h"
#include "game_data/character/collectible/collectible.h"
#include "game_data/character/skill/skill.h"
#include "ui/widget_manager.h"
#include "ui/character/item/equip_widget.h"
#include "ui/character/item/accessory_widget.h"
#include "ui/character/item/abilitystone_widget.h"
#include "ui/character/item/bracelet_widget.h"
#include "ui/character/item/gem_widget.h"
#include "ui/character/engrave/engrave_widget.h"
#include "ui/character/skill/skill_widget.h"
#include "ui/font_manager.h"
#include <QHBoxLayout>

CharacterWidget::CharacterWidget(QWidget* pParent, const Character* pCharacter) :
    QWidget(pParent),
    ui(new Ui::CharacterWidget),
    m_pCharacter(pCharacter),
    m_pAbilityStoneWidget(nullptr),
    m_pBraceletWidget(nullptr),
    m_pEngraveWidget(nullptr)
{
    ui->setupUi(this);

    setLayoutAlignments();
    setStyleSheets();
    setFonts();
    addProfileWidget();
    addEquipWidgets();
    addAccessoryWidgets();
    addAbilityStoneWidget();
    addBraceletWidget();
    addEngraveWidget();
    addGemWidgets();
    addSkillWidgets();
}

CharacterWidget::~CharacterWidget()
{
    for (QLabel* pLabel : m_labels)
        delete pLabel;
    for (QHBoxLayout* pLayout : m_hLayouts)
        delete pLayout;
    for (EquipWidget* pEquipWidget : m_equipWidgets)
        delete pEquipWidget;
    for (AccessoryWidget* pAccessoryWidget : m_accessoryWidgets)
        delete pAccessoryWidget;
    if (m_pAbilityStoneWidget != nullptr)
        delete m_pAbilityStoneWidget;
    if (m_pBraceletWidget != nullptr)
        delete m_pBraceletWidget;
    if (m_pEngraveWidget != nullptr)
        delete m_pEngraveWidget;
    for (GemWidget* pGemWidget : m_gemWidgets)
        delete pGemWidget;
    for (SkillWidget* pSkillWidget : m_skillWidgets)
        delete pSkillWidget;
    delete ui;
}

void CharacterWidget::setFonts()
{
    FontManager* pFontManager = FontManager::getInstance();
    QFont nanumBold10 = pFontManager->getFont(FontFamily::NanumSquareNeoBold, 10);
    QFont nanumBold12 = pFontManager->getFont(FontFamily::NanumSquareNeoBold, 12);

    ui->pbSibling->setFont(nanumBold10);
    ui->lbName->setFont(nanumBold10);
    ui->groupTitleGuild->setFont(nanumBold10);
    ui->lbTitle->setFont(nanumBold10);
    ui->lbGuild->setFont(nanumBold10);
    ui->groupServerClass->setFont(nanumBold10);
    ui->lbServer->setFont(nanumBold10);
    ui->lbClass->setFont(nanumBold10);
    ui->lbBattleLevel->setFont(nanumBold10);
    ui->lbExpLevel->setFont(nanumBold10);
    ui->lbItemLevel->setFont(nanumBold10);
    ui->groupAbility->setFont(nanumBold10);
    ui->lbCritical->setFont(nanumBold10);
    ui->lbSpecification->setFont(nanumBold10);
    ui->lbSwiftness->setFont(nanumBold10);
    ui->lbDomination->setFont(nanumBold10);
    ui->lbEndurance->setFont(nanumBold10);
    ui->lbExpertise->setFont(nanumBold10);
    ui->groupCard->setFont(nanumBold10);
    ui->groupCollectible->setFont(nanumBold10);
    ui->tabWidget->setFont(nanumBold10);
}

void CharacterWidget::setLayoutAlignments()
{
    ui->gridTabSkill->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    ui->vLayoutTabProfileEquip->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ui->hLayoutEquip->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui->hLayoutAccessory->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui->hLayoutStoneBraceletEngrave->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui->hLayoutGem1->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui->hLayoutGem2->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
}

void CharacterWidget::setStyleSheets()
{
    QString labelColor = "QLabel { color: %1 }";
    QString tabBackgroundColor = "QWidget { background-color: rgb(240, 240, 240) }";

    ui->lbGuild->setStyleSheet(labelColor.arg(COLOR_GUILD));
    ui->lbServer->setStyleSheet(labelColor.arg(COLOR_SERVER));
    ui->lbItemLevel->setStyleSheet(labelColor.arg(COLOR_ITEMLEVEL));
    ui->tabProfileEquip->setStyleSheet(tabBackgroundColor);
    ui->tabSkill->setStyleSheet(tabBackgroundColor);
}

void CharacterWidget::addProfileWidget()
{
    const Profile* pProfile = m_pCharacter->getProfile();
    ui->lbName->setText(pProfile->getCharacterName());
    ui->lbTitle->setText(pProfile->getTitle());
    ui->lbGuild->setText(pProfile->getGuild());
    ui->lbServer->setText("@" + pProfile->getServer());
    ui->lbClass->setText(classToStr(pProfile->getClass()));
    ui->lbBattleLevel->setText(QString("전투 Lv.%1").arg(pProfile->getCharacterLevel()));
    ui->lbExpLevel->setText(QString("원정대 Lv.%1").arg(pProfile->getExpLevel()));
    ui->lbItemLevel->setText(QString("아이템 Lv.%1").arg(pProfile->getItemLevel()));

    const QMap<Ability, int>& abilities = pProfile->getAbilities();
    ui->lbCritical->setText(abilityToStr(Ability::치명) + " " + QString::number(abilities[Ability::치명]));
    ui->lbSpecification->setText(abilityToStr(Ability::특화) + " " + QString::number(abilities[Ability::특화]));
    ui->lbSwiftness->setText(abilityToStr(Ability::신속) + " " + QString::number(abilities[Ability::신속]));
    ui->lbDomination->setText(abilityToStr(Ability::제압) + " " + QString::number(abilities[Ability::제압]));
    ui->lbEndurance->setText(abilityToStr(Ability::인내) + " " + QString::number(abilities[Ability::인내]));
    ui->lbExpertise->setText(abilityToStr(Ability::숙련) + " " + QString::number(abilities[Ability::숙련]));

    // Card
    const QList<Card*>& cards = m_pCharacter->getCards();
    for (const Card* pCard : cards)
    {
        // 최종완성단계 효과만 추가
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
            cardEffectText += QString(" (%1각)").arg(awaken);
        }
        else
        {
            cardEffectText = cardEffectName;
        }

        QLabel* pCardLabel = WidgetManager::createLabel(cardEffectText, 300, 25, 10, this);
        m_labels.append(pCardLabel);
        ui->vLayoutCard->addWidget(pCardLabel);
    }

    // Collectible
    const QList<Collectible*>& collectibles = m_pCharacter->getCollectibles();
    QString iconPath = ":/collectible/image/collectible/%1.png";
    QString pointText = "%1/%2";
    for (int i = 0; i < collectibles.size(); i++)
    {
        int row = i / 5;
        int col = (i % 5) * 2;
        const Collectible* pCollectible = collectibles[i];

        QLabel* pIcon = WidgetManager::createIcon(iconPath.arg(static_cast<int>(pCollectible->getType())), nullptr, 25, 25, this);
        pIcon->setStyleSheet("QLabel { background-color: black }");
        m_labels.append(pIcon);
        ui->gridGroupCollectible->addWidget(pIcon, row, col);

        QLabel* pLabelPoint = WidgetManager::createLabel(pointText.arg(pCollectible->getPoint()).arg(pCollectible->getMaxPoint()), 75, 25, 10, this);
        pLabelPoint->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_labels.append(pLabelPoint);
        ui->gridGroupCollectible->addWidget(pLabelPoint, row, col + 1);
    }
}

void CharacterWidget::addEquipWidgets()
{
    for (int i = 0; i < 6; i++) {
        const Equip* pEquip = m_pCharacter->getEquip(static_cast<ItemType>(i));
        if (pEquip == nullptr)
            continue;

        EquipWidget* pEquipWidget = new EquipWidget(this, pEquip);
        m_equipWidgets.append(pEquipWidget);
        ui->hLayoutEquip->addWidget(pEquipWidget);
    }
}

void CharacterWidget::addAccessoryWidgets()
{
    const Accessory* pNeck = m_pCharacter->getAccessory(ItemType::목걸이);
    if (pNeck != nullptr)
    {
        AccessoryWidget* pAccessoryWidget = new AccessoryWidget(this, pNeck);
        m_accessoryWidgets.append(pAccessoryWidget);
        ui->hLayoutAccessory->addWidget(pAccessoryWidget);
    }

    for (int i = 0; i < 2; i++)
    {
        const Accessory* pEar = m_pCharacter->getAccessory(ItemType::귀걸이, i);
        if (pEar != nullptr)
        {
            AccessoryWidget* pAccessoryWidget = new AccessoryWidget(this, pEar);
            m_accessoryWidgets.append(pAccessoryWidget);
            ui->hLayoutAccessory->addWidget(pAccessoryWidget);
        }

        const Accessory* pRing = m_pCharacter->getAccessory(ItemType::반지, i);
        if (pRing != nullptr)
        {
            AccessoryWidget* pAccessoryWidget = new AccessoryWidget(this, pRing);
            m_accessoryWidgets.append(pAccessoryWidget);
            ui->hLayoutAccessory->addWidget(pAccessoryWidget);
        }
    }
}

void CharacterWidget::addAbilityStoneWidget()
{
    const AbilityStone* pAbilityStone = m_pCharacter->getAbilityStone();
    if (pAbilityStone != nullptr)
    {
        m_pAbilityStoneWidget = new AbilityStoneWidget(this, pAbilityStone);
        ui->hLayoutStoneBraceletEngrave->addWidget(m_pAbilityStoneWidget);
    }
}

void CharacterWidget::addBraceletWidget()
{
    const Bracelet* pBracelet = m_pCharacter->getBracelet();
    if (pBracelet != nullptr)
    {
        m_pBraceletWidget = new BraceletWidget(this, pBracelet);
        ui->hLayoutStoneBraceletEngrave->addWidget(m_pBraceletWidget);
    }
}

void CharacterWidget::addEngraveWidget()
{
    const Engrave* pEngrave = m_pCharacter->getEngrave();
    if (pEngrave != nullptr)
    {
        m_pEngraveWidget = new EngraveWidget(this, pEngrave);
        ui->hLayoutStoneBraceletEngrave->addWidget(m_pEngraveWidget);
    }
}

void CharacterWidget::addGemWidgets()
{
    const QList<Gem*>& gems = m_pCharacter->getGems();
    int myulLevelSum = 0, myulCount = 0;
    int hongLevelSum = 0, hongCount = 0;
    for (const Gem* pGem : gems)
    {
        const GemType gemType = pGem->getGemType();
        GemWidget* pGemWidget = new GemWidget(this, pGem);
        m_gemWidgets.append(pGemWidget);
        if (gemType == GemType::멸화)
        {
            ui->hLayoutGem1->addWidget(pGemWidget);
            myulLevelSum += pGem->getLevel();
            myulCount++;
        }
        else if (gemType == GemType::홍염)
        {
            ui->hLayoutGem2->addWidget(pGemWidget);
            hongLevelSum += pGem->getLevel();
            hongCount++;
        }
    }

    const int LABEL_WIDTH = 100;
    const int LABEL_HEIGHT = 50;
    if (myulCount > 0)
    {
        QLabel* pLabelMyulAvg = WidgetManager::createLabel(QString("멸화 평균 레벨\n%1").arg(myulLevelSum / (double)myulCount, 0, 'f', 2, QChar(' ')), LABEL_WIDTH, LABEL_HEIGHT, 10, this);
        m_labels.append(pLabelMyulAvg);
        ui->hLayoutGem1->addWidget(pLabelMyulAvg);
    }
    if (hongCount > 0)
    {
        QLabel* pLabelHongAvg = WidgetManager::createLabel(QString("홍염 평균 레벨\n%1").arg(hongLevelSum / (double)hongCount, 0, 'f', 2, QChar(' ')), LABEL_WIDTH, LABEL_HEIGHT, 10, this);
        m_labels.append(pLabelHongAvg);
        ui->hLayoutGem2->addWidget(pLabelHongAvg);
    }
}

void CharacterWidget::addSkillWidgets()
{
    const QList<Skill*>& skills = m_pCharacter->getSkills();
    int tripodLevels[2] = {0, 0};

    // SkillWidget 추가
    for (int i = 0; i < skills.size(); i++)
    {
        const Skill* pSkill = skills[i];

        SkillWidget* pSkillWidget = new SkillWidget(this, pSkill);
        m_skillWidgets.append(pSkillWidget);
        ui->gridTabSkill->addWidget(pSkillWidget, (i / 3) + 3, i % 3);

        const QList<Tripod>& tripods = pSkill->getTripods();
        for (const Tripod& tripod : tripods)
        {
            int tripodLevel = tripod.level;
            if (tripodLevel == 4)
                tripodLevels[0]++;
            else if (tripodLevel == 5)
                tripodLevels[1]++;
        }
    }

    // 트라이포드 활성화 정보 추가
    const int LABEL_WIDTH = 250;
    const int LABEL_HEIGHT = 25;

    QString tripodText = "트라이포드 활성화 (4레벨 이상) : (%1 / 18)";
    QLabel* pLabelTripod = WidgetManager::createLabel(tripodText.arg(tripodLevels[0] + tripodLevels[1]), LABEL_WIDTH, LABEL_HEIGHT, 10, this);
    m_labels.append(pLabelTripod);
    ui->gridTabSkill->addWidget(pLabelTripod, 0, 0);

    for (int i = 0; i < 2; i++)
    {
        QString tripodLevel = "Lv.%1 (%2개)";
        QLabel* pLabelTripodLevel = WidgetManager::createLabel(tripodLevel.arg(i + 4).arg(tripodLevels[i]), LABEL_WIDTH, LABEL_HEIGHT, 10, this);
        m_labels.append(pLabelTripodLevel);
        ui->gridTabSkill->addWidget(pLabelTripodLevel, i + 1, 0);
    }
}
