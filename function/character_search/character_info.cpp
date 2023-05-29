#include "character_info.h"
#include "ui_character_info.h"
#include "ui/widget_manager.h"
#include "game/engrave/engrave_manager.h"
#include "function/character_search/character_search.h"
#include "function/character_search/accessory_info.h"
#include "function/character_search/abilitystone_info.h"
#include "function/character_search/bracelet_info.h"
#include "function/character_search/equip_info.h"
#include "function/character_search/gem_info.h"
#include "function/character_search/skill_info.h"
#include "resource/resource_manager.h"

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
    initializeGemLayout(pCharacter->gems());
    initializeSkillLayout(pCharacter->skills(),
                          pCharacter->profile().usingSkillPoint,
                          pCharacter->profile().totalSkillPoint);
}

CharacterInfo::~CharacterInfo()
{
    delete ui;
}

void CharacterInfo::initializeProfileLayout(const Character *pCharacter)
{
    ui->vLayoutProfile->setAlignment(Qt::AlignTop);

    addSiblingSelector(pCharacter->siblings(), pCharacter->profile().characterName);
    addProfileInfo(pCharacter->profile());
    addAbilityInfo(pCharacter->profile().stats);
    addEngraveInfo(pCharacter->engraves());
    addCardInfo(pCharacter->cards());
    addCollectibleInfo(pCharacter->collectibles());
}

void CharacterInfo::addSiblingSelector(const QList<Profile> &siblings, const QString &characterName)
{
    QStringList characterList = {QString("보유 캐릭터\n(%1)").arg(siblings.size() + 1)};
    QString siblingText = "[%1 Lv.%2]\n%3";

    for (const Profile &sibling : siblings)
    {
        if (sibling.characterName != characterName)
        {
            characterList << siblingText
                                 .arg(sibling.className)
                                 .arg(sibling.itemLevel)
                                 .arg(sibling.characterName);
        }
    }

    QComboBox *pSiblingSelector = WidgetManager::createComboBox(
        characterList, 10, 200, 50);

    pSiblingSelector->setFixedHeight(50);
    ui->vLayoutProfile->addWidget(pSiblingSelector);

    for (int i = 0; i < pSiblingSelector->count(); i++)
    {
        pSiblingSelector->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
    }

    connect(pSiblingSelector, &QComboBox::currentIndexChanged,
            this, [&, siblings](int index)
    {
        if (index != 0)
        {
            emit CharacterSearch::getInstance()->searchRequested(
                siblings.at(index - 1).characterName);
        }
    });
}

void CharacterInfo::addProfileInfo(const Profile &profile)
{
    const QStringList profileTexts =
        {
            QString("@%1").arg(profile.serverName),
            profile.className,
            profile.characterName,
            QString("원정대 Lv.%1").arg(profile.expeditionLevel),
            QString("전투 Lv.%1").arg(profile.characterLevel),
            QString("아이템 Lv.%1").arg(profile.itemLevel),
            QString("공격력 : %L1").arg(profile.stats["공격력"]),
            QString("최대 생명력 : %L1").arg(profile.stats["최대 생명력"])
        };
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

void CharacterInfo::addAbilityInfo(const QHash<QString, int> &abilities)
{
    const QStringList keys = {"치명", "특화", "신속", "제압", "인내", "숙련"};

    addLayoutTitle("특성", ui->vLayoutProfile);

    for (const QString &key : keys)
    {
        QLabel *pLabelAbility = WidgetManager::createLabel(
            QString("%1 : %2").arg(key).arg(abilities[key]), 12);

        ui->vLayoutProfile->addWidget(pLabelAbility);
    }
}

void CharacterInfo::addEngraveInfo(const QList<QPair<QString, int>> &engraves)
{
    EngraveManager *pEngraveManager = EngraveManager::getInstance();

    addLayoutTitle("각인", ui->vLayoutProfile);

    for (int i = 0; i < engraves.size(); i++)
    {
        // 각인 레벨별 레이아웃 생성 (레벨 3 -> 2 -> 1 순으로 추가되도록)
        QList<QVBoxLayout*> vLayouts(3, nullptr);

        for (int j = 2; j >= 0; j--)
        {
            QVBoxLayout* pVLayout = new QVBoxLayout();
            ui->vLayoutProfile->addLayout(pVLayout);
            vLayouts[j] = pVLayout;
        }

        // 각인 정보 추가
        QHBoxLayout *pHLayout = new QHBoxLayout();
        vLayouts[engraves[i].second - 1]->addLayout(pHLayout);

        QLabel *pEngraveIcon = WidgetManager::createIcon(
            pEngraveManager->iconPath(engraves[i].first), nullptr);
        pHLayout->addWidget(pEngraveIcon);

        QLabel *pLabelEngraveLevel = WidgetManager::createLabel(
            QString("Lv.%1").arg(engraves[i].second), 10, 50);

        if (engraves[i].first.contains("감소")) {
            pLabelEngraveLevel->setStyleSheet("QLabel { color: red }");
        }

        pHLayout->addWidget(pLabelEngraveLevel);
    }
}

void CharacterInfo::addCardInfo(const QList<QPair<QString, int>> &cards)
{
    addLayoutTitle("카드", ui->vLayoutProfile);

    for (int i = 0; i < cards.size(); i++)
    {
        QLabel *pLabelCardSet = WidgetManager::createLabel(
            QString("%1 (%2각)").arg(cards[i].first).arg(cards[i].second), 10);

        ui->vLayoutProfile->addWidget(pLabelCardSet);
    }
}

void CharacterInfo::addCollectibleInfo(const QList<Collectible> &collectibles)
{
    ResourceManager *pResourceManager = ResourceManager::getInstance();

    addLayoutTitle("수집품", ui->vLayoutProfile);

    QString pointText = "%1 / %2";

    for (const Collectible &collectible : collectibles)
    {
        QHBoxLayout *pHLayout = new QHBoxLayout();
        ui->vLayoutProfile->addLayout(pHLayout);

        QLabel *pIcon = WidgetManager::createIcon(
            pResourceManager->iconPath(collectible.type), nullptr, "black", 25, 25);
        pHLayout->addWidget(pIcon);

        QLabel *pLabelPoint = WidgetManager::createLabel(
            pointText.arg(collectible.point).arg(collectible.maxPoint));
        pHLayout->addWidget(pLabelPoint);
    }
}

void CharacterInfo::initializeEquipLayout(const Character *pCharacter)
{
    ui->vLayoutEquip->setAlignment(Qt::AlignTop);

    addLayoutTitle("장비", ui->vLayoutEquip);

    addArmorInfo(pCharacter->armors());
    addWeaponInfo(pCharacter->weapon());
    addElixirInfo(pCharacter->armors());
}

void CharacterInfo::addArmorInfo(const QList<Equipment> &armors)
{
    for (int i = 0; i < armors.size(); i++)
    {
        EquipInfo *pArmorInfo = new EquipInfo(armors[i]);

        ui->vLayoutEquip->addWidget(pArmorInfo);
        ui->vLayoutEquip->setAlignment(pArmorInfo, Qt::AlignLeft);
    }
}

void CharacterInfo::addWeaponInfo(const Equipment &weapon)
{
    EquipInfo *pWeaponInfo = new EquipInfo(weapon);

    ui->vLayoutEquip->addWidget(pWeaponInfo);
    ui->vLayoutEquip->setAlignment(pWeaponInfo, Qt::AlignLeft);
}

void CharacterInfo::addElixirInfo(const QList<Equipment> &armors)
{
    int elixirLevelSum = 0;
    QString elixirEffectHead = "질서";
    QString elixirEffectHand = "혼돈";

    for (const Equipment &armor : armors)
    {
        if (armor.type == "")
            continue;

        for (auto iter = armor.elixirs.constBegin(); iter != armor.elixirs.constEnd(); iter++)
        {
            // 엘릭서 레벨 합계 계산
            elixirLevelSum += iter.value();

            // 투구, 장갑인 경우 부위 전용 효과 추출
            if (armor.type == "투구" && iter.key().contains("("))
                elixirEffectHead = iter.key().sliced(0, iter.key().indexOf("(") - 1);
            else if (armor.type == "장갑" && iter.key().contains("("))
                elixirEffectHand = iter.key().sliced(0, iter.key().indexOf("(") - 1);
        }
    }

    addLayoutTitle("엘릭서 정보", ui->vLayoutEquip);

    // 엘릭서 연성 레벨 합 정보 추가
    QLabel *pLabelTotalLevel = WidgetManager::createLabel(
        QString("연성 레벨 합 : %1").arg(elixirLevelSum), 12);
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
                QLabel *pLabelSetLevel = WidgetManager::createLabel(
                    QString("%1 %2단계 활성화").arg(elixirEffectHead).arg(i + 1), 12);
                pLabelSetLevel->setStyleSheet(
                    QString("QLabel { color: %1 }").arg(
                        itemGradeToTextColor(ItemGrade::고급)));
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
    addAccessoryInfo(pCharacter->accessories());

    addLayoutTitle("어빌리티 스톤", ui->vLayoutAccessory);
    addAbilityStoneInfo(pCharacter->abilityStone());

    addLayoutTitle("팔찌", ui->vLayoutAccessory);
    addBraceletInfo(pCharacter->bracelet());
}

void CharacterInfo::addAccessoryInfo(const QList<Equipment> &accessories)
{
    for (const Equipment &accessory : accessories)
    {
        AccessoryInfo *pAccInfo = new AccessoryInfo(accessory);

        ui->vLayoutAccessory->addWidget(pAccInfo);
        ui->vLayoutAccessory->setAlignment(pAccInfo, Qt::AlignLeft);
    }
}

void CharacterInfo::addAbilityStoneInfo(const Equipment &abilityStone)
{
    AbilityStoneInfo *pAbilityStoneInfo = new AbilityStoneInfo(abilityStone);

    ui->vLayoutAccessory->addWidget(pAbilityStoneInfo);
    ui->vLayoutAccessory->setAlignment(pAbilityStoneInfo, Qt::AlignLeft);
}

void CharacterInfo::addBraceletInfo(const Equipment &bracelet)
{
    BraceletInfo *pBraceletInfo = new BraceletInfo(bracelet);

    ui->vLayoutAccessory->addWidget(pBraceletInfo);
    ui->vLayoutAccessory->setAlignment(pBraceletInfo, Qt::AlignLeft);
}

void CharacterInfo::initializeGemLayout(const QList<Gem> &gems)
{
    ui->vLayoutGem1->setAlignment(Qt::AlignTop);
    ui->vLayoutGem2->setAlignment(Qt::AlignTop);

    addLayoutTitle("멸화 보석", ui->vLayoutGem1);
    addLayoutTitle("홍염 보석", ui->vLayoutGem2);

    addGemInfo(gems);
    addGemLevelAvgInfo(gems);
}

void CharacterInfo::addGemInfo(const QList<Gem> &gems)
{
    for (const Gem &gem : gems)
    {
        QVBoxLayout *pVLayout = nullptr;

        if (gem.type == "멸화")
            pVLayout = ui->vLayoutGem1;
        else if (gem.type == "홍염")
            pVLayout = ui->vLayoutGem2;
        else
            continue;

        GemInfo *pGemInfo = new GemInfo(gem);

        pVLayout->addWidget(pGemInfo);
        pVLayout->setAlignment(pGemInfo, Qt::AlignLeft);
    }
}

void CharacterInfo::addGemLevelAvgInfo(const QList<Gem> &gems)
{
    // 0: 멸화, 1: 홍염
    int levelSum[] = {0, 0};
    int count[] = {0, 0};

    // 보석 타입별 레벨 및 갯수 계산
    for (const Gem &gem : gems)
    {
        int index = 0;

        if (gem.type == "멸화")
            index = 0;
        else if (gem.type == "홍염")
            index = 1;
        else
            continue;

        count[index]++;
        levelSum[index] += gem.gemLevel;
    }

    // 보석 타입별 평균 레벨 정보 추가
    QList<QVBoxLayout*> layouts = {ui->vLayoutGem1, ui->vLayoutGem2};

    for (int i = 0; i < layouts.size(); i++)
    {
        QString typeStr = i == 0 ? "멸화" : "홍염";

        addLayoutTitle(QString("%1 평균 레벨").arg(typeStr), layouts[i]);

        if (count[i] > 0)
        {
            QLabel *pLabelGemLevelAvg = WidgetManager::createLabel(
                QString::number(levelSum[i] / (double)count[i], 'f', 2), 12);
            layouts[i]->addWidget(pLabelGemLevelAvg);
        }
    }
}

void CharacterInfo::initializeSkillLayout(const QList<Skill> &skills, int usingSkillPoint, int totalSkillPoint)
{
    ui->vLayoutSkill->setAlignment(Qt::AlignTop);

    addTripodLevelInfo(skills);

    const QString titleText = QString("스킬 (%1/%2)")
                                  .arg(usingSkillPoint)
                                  .arg(totalSkillPoint);

    addSkillInfo(titleText, skills);
}

void CharacterInfo::addTripodLevelInfo(const QList<Skill> &skills)
{
    addLayoutTitle("트라이포드 활성화 정보", ui->vLayoutSkill);

    // 4, 5레벨 트라이포드 활성화 갯수 counting
    QList<int> enableCount = {0, 0};

    for (const Skill &skill : skills)
    {
        for (const Tripod &tripod : skill.tripods)
        {
            if (tripod.tripodLevel == 4)
                enableCount[0]++;
            else if (tripod.tripodLevel == 5)
                enableCount[1]++;
        }
    }

    // 4, 5레벨 트라이포드 활성화 정보 추가
    const int MAX_TRIPOD = 18;

    QLabel *pLabelTripodTotal = WidgetManager::createLabel(
                                        QString("4, 5레벨 트라이포드 활성화 (%1 / %2)")
                                        .arg(enableCount[0] + enableCount[1])
                                        .arg(MAX_TRIPOD), 12, 300);
    ui->vLayoutSkill->addWidget(pLabelTripodTotal);
    ui->vLayoutSkill->setAlignment(pLabelTripodTotal, Qt::AlignHCenter);

    QHBoxLayout *pHLayout = new QHBoxLayout();
    pHLayout->setAlignment(Qt::AlignHCenter);
    ui->vLayoutSkill->addLayout(pHLayout);

    for (int i = 0; i < enableCount.size(); i++)
    {
        QLabel *pLabelTripod = WidgetManager::createLabel(
            QString("%1레벨 : %2").arg(i + 4).arg(enableCount[i]));
        pLabelTripod->setStyleSheet("QLabel { border: 1px solid black;"
                                    "         border-radius: 5px;"
                                    "         padding: 2px }");
        pHLayout->addWidget(pLabelTripod);
    }
}

void CharacterInfo::addSkillInfo(const QString &title, const QList<Skill> &skills)
{
    // title 추가
    addLayoutTitle(title, ui->vLayoutSkill);

    // 스킬 정보 추가
    int count = 0;

    for (const Skill &skill : skills)
    {
        // 스킬레벨을 올렸거나 룬을 착용한 스킬만 표시
        if (skill.skillLevel > 1 || skill.pRune != nullptr)
        {
            if (count != 0)
                addHLine(ui->vLayoutSkill);

            SkillInfo *pSkillInfo = new SkillInfo(skill);
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
