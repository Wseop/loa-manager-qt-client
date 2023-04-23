#include "character_info.h"
#include "ui_character_info.h"
#include "ui/widget_manager.h"
#include "game/engrave/engrave_manager.h"
#include "function/character_search/character_search.h"
#include "function/character_search/equip_info.h"
#include "function/character_search/accessory_info.h"
#include "function/character_search/abilitystone_info.h"
#include "function/character_search/bracelet_info.h"
#include "function/character_search/gem_info.h"
#include "function/character_search/skill_info.h"

#include <QComboBox>
#include <QLabel>
#include <QFrame>

CharacterInfo::CharacterInfo(Character *pCharacter) :
    ui(new Ui::CharacterInfo)
{
    ui->setupUi(this);
    ui->hLayoutMain->setAlignment(Qt::AlignTop);
    ui->hLayoutContent->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    initializeProfileLayout(pCharacter);
    initializeEquipLayout(pCharacter);
    initializeAccessoryLayout(pCharacter);
    initializeGemLayout(pCharacter->getGems());
    initializeSkillLayout(pCharacter);
}

CharacterInfo::~CharacterInfo()
{
    delete ui;
}

void CharacterInfo::initializeProfileLayout(const Character *pCharacter)
{
    ui->vLayoutProfile->setAlignment(Qt::AlignTop);

    addSiblingSelector(pCharacter->getSiblings(), pCharacter->getProfile());
    addProfileInfo(pCharacter->getProfile());
    addAbilityInfo(pCharacter->getProfile()->getAbility());
    addEngraveInfo(pCharacter->getEngrave());
    addCardInfo(pCharacter->getCard());
}

void CharacterInfo::addSiblingSelector(const QList<Profile*> &siblings, const Profile *pProfile)
{
    QStringList characterList = { QString("보유 캐릭터\n(%1)").arg(siblings.size() + 1) };
    QString itemText = "[%1 Lv.%2]\n%3";

    for (const Profile *pSibling : siblings)
    {
        if (pSibling->getCharacterName() != pProfile->getCharacterName())
        {
            characterList << itemText.arg(pSibling->getCharacterClass())
                                     .arg(pSibling->getItemLevel())
                                     .arg(pSibling->getCharacterName());
        }
    }

    QComboBox *pSiblingSelector = WidgetManager::createComboBox(characterList, 10, 200, 50);
    pSiblingSelector->setFixedHeight(50);
    ui->vLayoutProfile->addWidget(pSiblingSelector);

    for (int i = 0; i < pSiblingSelector->count(); i++)
    {
        pSiblingSelector->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
    }

    connect(pSiblingSelector, &QComboBox::currentIndexChanged, this, [&, siblings](int index)
    {
        if (index != 0)
        {
            emit CharacterSearch::getInstance()->searchRequested(siblings.at(index - 1)->getCharacterName());
        }
    });
}

void CharacterInfo::addProfileInfo(const Profile *pProfile)
{
    const QStringList profileTexts = { QString("@%1").arg(pProfile->getServerName()),
                                     pProfile->getCharacterClass(),
                                     pProfile->getCharacterName(),
                                     QString("원정대 Lv.%1").arg(pProfile->getExpeditionLevel()),
                                     QString("전투 Lv.%1").arg(pProfile->getCharacterLevel()),
                                     QString("아이템 Lv.%1").arg(pProfile->getItemLevel()),
                                     QString("공격력 : %L1").arg(pProfile->getAttack()),
                                     QString("최대 생명력 : %L1").arg(pProfile->getMaxHp()) };
    const QString labelStyle = "QLabel { color: %1 }";

    for (int i = 0; i < profileTexts.size(); i++)
    {
        if (i % 3 == 0)
            addHLine(ui->vLayoutProfile);

        QLabel *pLabel = WidgetManager::createLabel(profileTexts[i], 12);
        ui->vLayoutProfile->addWidget(pLabel);

        if (i == 0)
            pLabel->setStyleSheet(labelStyle.arg("#B178FF"));
        else if (i == 5)
            pLabel->setStyleSheet(labelStyle.arg("#FF009B"));
    }
}

void CharacterInfo::addAbilityInfo(const QHash<Ability, int> &abilities)
{
    const QString text = "%1 : %2";

    addLayoutTitle("특성", ui->vLayoutProfile);

    for (int i = 0; i < static_cast<int>(Ability::size); i++)
    {
        Ability ability = static_cast<Ability>(i);

        QLabel *pLabelAbility = WidgetManager::createLabel(text.arg(abilityToQString(ability)).arg(abilities[ability]), 12);
        ui->vLayoutProfile->addWidget(pLabelAbility);
    }
}

void CharacterInfo::addEngraveInfo(const Engrave *pEngrave)
{
    EngraveManager *pEngraveManager = EngraveManager::getInstance();

    addLayoutTitle("각인", ui->vLayoutProfile);

    if (pEngrave == nullptr)
        return;

    const QList<QStringList> engraves = {pEngrave->getEngraves(), pEngrave->getPenalties()};

    for (int i = 0; i < engraves.size(); i++)
    {
        QString textColor = (i == 0) ? "" : "red";

        // 각인 레벨별 레이아웃 생성 (레벨 3 -> 2 -> 1 순으로 추가되도록)
        QList<QVBoxLayout*> vLayouts(3, nullptr);

        for (int j = 2; j >= 0; j--)
        {
            QVBoxLayout* pVLayout = new QVBoxLayout();
            ui->vLayoutProfile->addLayout(pVLayout);
            vLayouts[j] = pVLayout;
        }

        // 각인 정보 추가
        for (const QString &engrave : engraves[i])
        {
            int level = (i == 0) ? pEngrave->getEngraveLevel(engrave) : pEngrave->getPenaltyLevel(engrave);

            QHBoxLayout *pHLayout = new QHBoxLayout();
            vLayouts[level - 1]->addLayout(pHLayout);

            QLabel *pEngraveIcon = WidgetManager::createIcon(pEngraveManager->iconPath(engrave), nullptr);
            pHLayout->addWidget(pEngraveIcon);

            QLabel *pLabelEngraveLevel = WidgetManager::createLabel(QString("Lv.%1").arg(level), 10, 50);
            pLabelEngraveLevel->setStyleSheet(QString("QLabel { color: %1 }").arg(textColor));
            pHLayout->addWidget(pLabelEngraveLevel);
        }
    }
}

void CharacterInfo::addCardInfo(const Card *pCard)
{
    QString text = "%1 (%2각)";

    addLayoutTitle("카드", ui->vLayoutProfile);

    if (pCard == nullptr)
        return;

    for (const QPair<QString, int> &cardSet : pCard->cardSets())
    {
        QLabel *pLabelCardSet = WidgetManager::createLabel(text.arg(cardSet.first).arg(cardSet.second), 10);
        ui->vLayoutProfile->addWidget(pLabelCardSet);
    }
}

void CharacterInfo::initializeEquipLayout(const Character *pCharacter)
{
    ui->vLayoutEquip->setAlignment(Qt::AlignTop);

    addLayoutTitle("장비", ui->vLayoutEquip);

    addArmorInfo(pCharacter->getArmors());
    addWeaponInfo(pCharacter->getWeapon());
    addElixirInfo(pCharacter->getArmors());
}

void CharacterInfo::addArmorInfo(const QList<Armor *> &armors)
{
    for (int i = 0; i < armors.size(); i++)
    {
        EquipInfo *pArmorInfo = new EquipInfo(armors[i]);

        ui->vLayoutEquip->addWidget(pArmorInfo);
        ui->vLayoutEquip->setAlignment(pArmorInfo, Qt::AlignLeft);
    }
}

void CharacterInfo::addWeaponInfo(const Weapon *pWeapon)
{
    EquipInfo *pWeaponInfo = new EquipInfo(pWeapon);

    ui->vLayoutEquip->addWidget(pWeaponInfo);
    ui->vLayoutEquip->setAlignment(pWeaponInfo, Qt::AlignLeft);
}

void CharacterInfo::addElixirInfo(const QList<Armor *> &armors)
{
    int elixirLevelSum = 0;
    QString elixirEffectHead = "질서";
    QString elixirEffectHand = "혼돈";

    for (const Armor *pArmor : armors)
    {
        if (pArmor == nullptr)
            continue;

        for (const Elixir &elixir : pArmor->elixirs())
        {
            // 활성화된 엘릭서 레벨 합계 계산
            if (pArmor->armorPart() == elixir.part || elixir.part == ArmorPart::size)
                elixirLevelSum += elixir.level;

            // 투구, 장갑인 경우 부위 전용 효과 추출
            if (pArmor->armorPart() == ArmorPart::Head && elixir.effect.contains("("))
                elixirEffectHead = elixir.effect.sliced(0, elixir.effect.indexOf("(") - 1);
            else if (pArmor->armorPart() == ArmorPart::Hand && elixir.effect.contains("("))
                elixirEffectHand = elixir.effect.sliced(0, elixir.effect.indexOf("(") - 1);
        }
    }

    addLayoutTitle("엘릭서 정보", ui->vLayoutEquip);

    // 엘릭서 연성 레벨 합 정보 추가
    QLabel *pLabelTotalLevel = WidgetManager::createLabel(QString("연성 레벨 합 : %1").arg(elixirLevelSum), 12);
    ui->vLayoutEquip->addWidget(pLabelTotalLevel);
    ui->vLayoutEquip->setAlignment(pLabelTotalLevel, Qt::AlignHCenter);

    // 엘릭서 세트효과 활성화 정보 추가
    const QList<int> enableLevels = {35, 40};

    if (elixirEffectHead == elixirEffectHand)
    {
        for (int i = 0; i < enableLevels.size(); i++)
        {
            if (elixirLevelSum >= enableLevels[i])
            {
                QLabel *pLabelSetLevel = WidgetManager::createLabel(QString("%1 %2단계 활성화").arg(elixirEffectHead).arg(i + 1), 12);
                pLabelSetLevel->setStyleSheet(QString("QLabel { color: %1 }").arg(itemGradeToTextColor(ItemGrade::고급)));
                ui->vLayoutEquip->addWidget(pLabelSetLevel);
                ui->vLayoutEquip->setAlignment(pLabelSetLevel, Qt::AlignHCenter);
            }
        }
    }
}

void CharacterInfo::initializeAccessoryLayout(const Character *pCharacter)
{
    ui->vLayoutAccessory->setAlignment(Qt::AlignTop);

    addLayoutTitle("악세", ui->vLayoutAccessory);
    addAccessoryInfo(pCharacter->getAccessories());

    addLayoutTitle("어빌리티 스톤", ui->vLayoutAccessory);
    addAbilityStoneInfo(pCharacter->getAbilityStone());

    addLayoutTitle("팔찌", ui->vLayoutAccessory);
    addBraceletInfo(pCharacter->getBracelet());
}

void CharacterInfo::addAccessoryInfo(const QList<Accessory *> &accessories)
{
    for (const Accessory *pAccessory : accessories)
    {
        AccessoryInfo *pAccInfo = new AccessoryInfo(pAccessory);

        ui->vLayoutAccessory->addWidget(pAccInfo);
        ui->vLayoutAccessory->setAlignment(pAccInfo, Qt::AlignLeft);
    }
}

void CharacterInfo::addAbilityStoneInfo(const AbilityStone *pAbilityStone)
{
    AbilityStoneInfo *pAbilityStoneInfo = new AbilityStoneInfo(pAbilityStone);

    ui->vLayoutAccessory->addWidget(pAbilityStoneInfo);
    ui->vLayoutAccessory->setAlignment(pAbilityStoneInfo, Qt::AlignLeft);
}

void CharacterInfo::addBraceletInfo(const Bracelet *pBracelet)
{
    BraceletInfo *pBraceletInfo = new BraceletInfo(pBracelet);

    ui->vLayoutAccessory->addWidget(pBraceletInfo);
    ui->vLayoutAccessory->setAlignment(pBraceletInfo, Qt::AlignLeft);
}

void CharacterInfo::initializeGemLayout(const QList<Gem*> &gems)
{
    ui->vLayoutGem1->setAlignment(Qt::AlignTop);
    ui->vLayoutGem2->setAlignment(Qt::AlignTop);

    addLayoutTitle("멸화 보석", ui->vLayoutGem1);
    addLayoutTitle("홍염 보석", ui->vLayoutGem2);

    addGemInfo(gems);
    addGemLevelAvgInfo(gems);
}

void CharacterInfo::addGemInfo(const QList<Gem *> &gems)
{
    for (const Gem *pGem : gems)
    {
        GemInfo *pGemInfo = new GemInfo(pGem);
        QVBoxLayout *pVLayout = nullptr;

        if (pGem->gemType() == GemType::멸화)
            pVLayout = ui->vLayoutGem1;
        else if (pGem->gemType() == GemType::홍염)
            pVLayout = ui->vLayoutGem2;

        // 비정상적인 보석 타입인 경우 제외
        if (pVLayout == nullptr)
        {
            delete pGemInfo;
            continue;
        }

        pVLayout->addWidget(pGemInfo);
        pVLayout->setAlignment(pGemInfo, Qt::AlignLeft);
    }
}

void CharacterInfo::addGemLevelAvgInfo(const QList<Gem *> &gems)
{
    int levelSum[] = {0, 0};
    int count[] = {0, 0};

    // 보석 타입별 레벨 및 갯수 계산
    for (const Gem *pGem : gems)
    {
        int index = static_cast<int>(pGem->gemType());

        if (index < 0 || index > 2)
            continue;

        count[index]++;
        levelSum[index] += pGem->gemLevel();
    }

    // 보석 타입별 평균 레벨 정보 추가
    QList<QVBoxLayout*> layouts = {ui->vLayoutGem1, ui->vLayoutGem2};

    for (int i = 0; i < layouts.size(); i++)
    {
        QString typeStr = i == 0 ? "멸화" : "홍염";

        addLayoutTitle(QString("%1 평균 레벨").arg(typeStr), layouts[i]);

        if (count[i] > 0)
        {
            QLabel *pLabelGemLevelAvg = WidgetManager::createLabel(QString::number(levelSum[i] / (double)count[i], 'f', 2), 12);
            layouts[i]->addWidget(pLabelGemLevelAvg);
        }
    }
}

void CharacterInfo::initializeSkillLayout(const Character *pCharacter)
{
    ui->vLayoutSkill->setAlignment(Qt::AlignTop);

    addTripodLevelInfo(pCharacter->getSkills());

    const Profile *pProfile = pCharacter->getProfile();
    const QString titleText = QString("스킬 (%1/%2)").arg(pProfile->getUsingSkillPoint())
                                                    .arg(pProfile->getTotalSkillPoint());

    addSkillInfo(titleText, pCharacter->getSkills());
}

void CharacterInfo::addTripodLevelInfo(const QList<Skill *> &skills)
{
    addLayoutTitle("트라이포드 활성화 정보", ui->vLayoutSkill);

    // 4, 5레벨 트라이포드 활성화 갯수 counting
    QList<int> enableCount = {0, 0};

    for (const Skill *pSkill : skills)
    {
        for (const Tripod &tripod : pSkill->tripods())
        {
            if (tripod.isSelected())
            {
                if (tripod.tripodLevel() == 4)
                    enableCount[0]++;
                else if (tripod.tripodLevel() == 5)
                    enableCount[1]++;
            }
        }
    }

    // 4, 5레벨 트라이포드 활성화 정보 추가
    const int MAX_TRIPOD = 18;

    QLabel *pLabelTripodTotal = WidgetManager::createLabel(QString("4, 5레벨 트라이포드 활성화 (%1 / %2)")
                                                           .arg(enableCount[0] + enableCount[1])
                                                           .arg(MAX_TRIPOD), 12, 300);
    ui->vLayoutSkill->addWidget(pLabelTripodTotal);
    ui->vLayoutSkill->setAlignment(pLabelTripodTotal, Qt::AlignHCenter);

    QHBoxLayout *pHLayout = new QHBoxLayout();
    pHLayout->setAlignment(Qt::AlignHCenter);
    ui->vLayoutSkill->addLayout(pHLayout);

    for (int i = 0; i < enableCount.size(); i++)
    {
        QLabel *pLabelTripod = WidgetManager::createLabel(QString("%1레벨 : %2").arg(i + 4).arg(enableCount[i]));
        pLabelTripod->setStyleSheet("QLabel { border: 1px solid black;"
                                    "         border-radius: 5px;"
                                    "         padding: 2px }");
        pHLayout->addWidget(pLabelTripod);
    }
}

void CharacterInfo::addSkillInfo(const QString &title, const QList<Skill *> &skills)
{
    // title 추가
    addLayoutTitle(title, ui->vLayoutSkill);

    // 스킬 정보 추가
    int count = 0;

    for (const Skill *pSkill : skills)
    {
        // 스킬레벨을 올렸거나 룬을 착용한 스킬만 표시
        if (pSkill->skillLevel() > 1 || pSkill->rune() != nullptr)
        {
            if (count != 0)
                addHLine(ui->vLayoutSkill);

            SkillInfo *pSkillInfo = new SkillInfo(pSkill);
            ui->vLayoutSkill->addWidget(pSkillInfo);

            count++;
        }
    }
}

void CharacterInfo::addHLine(QLayout *pLayout)
{
    QFrame *pHLine = WidgetManager::createLine(QFrame::HLine);
    pLayout->addWidget(pHLine);
}

void CharacterInfo::addLayoutTitle(const QString &title, QLayout *pLayout)
{
    QLabel *pLabelTitle = WidgetManager::createLabel(title, 12, 1000);
    pLabelTitle->setStyleSheet("QLabel { border-radius: 5px;"
                               "         padding: 2px;"
                               "         background-color: black; "
                               "         color: white; }");
    pLayout->addWidget(pLabelTitle);
}
