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
    ui(new Ui::CharacterInfo),
    mpCharacter(pCharacter)
{
    ui->setupUi(this);
    ui->hLayoutMain->setAlignment(Qt::AlignTop);
    ui->hLayoutContent->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    initializeProfileLayout();
    initializeEquipLayout();
    initializeAccessoryLayout();
    initializeGemLayout();
    initializeSkillLayout();
}

CharacterInfo::~CharacterInfo()
{
    for (QWidget *pWidget : mWidgets)
        delete pWidget;
    mWidgets.clear();

    for (auto rIter = mLayouts.rbegin(); rIter != mLayouts.rend(); rIter++)
        delete *rIter;
    mLayouts.clear();

    delete ui;
}

void CharacterInfo::initializeProfileLayout()
{
    ui->vLayoutProfile->setAlignment(Qt::AlignTop);

    // 보유 캐릭터 추가
    addSiblingSelector(mpCharacter->getSiblings());

    // 프로필 정보 추가
    addProfileInfo(mpCharacter->getProfile());

    // 각인 정보 추가
    addEngraveInfo(mpCharacter->getEngrave());

    // 카드 정보 추가
    addCardInfo(mpCharacter->getCard());
}

void CharacterInfo::addProfileInfo(const Profile *pProfile)
{
    const QStringList texts = {QString("@%1").arg(pProfile->getServerName()),
                               pProfile->getCharacterClass(),
                               pProfile->getCharacterName(),
                               QString("원정대 Lv.%1").arg(pProfile->getExpeditionLevel()),
                               QString("전투 Lv.%1").arg(pProfile->getCharacterLevel()),
                               QString("아이템 Lv.%1").arg(pProfile->getItemLevel()),
                               QString("공격력 : %L1").arg(pProfile->getAttack()),
                               QString("최대 생명력 : %L1").arg(pProfile->getMaxHp())};
    QString labelStyle = "QLabel { color: %1 }";

    for (int i = 0; i < texts.size(); i++)
    {
        if (i % 3 == 0)
            addHLine(ui->vLayoutProfile);

        QLabel *pLabel = WidgetManager::createLabel(texts[i], 12);
        ui->vLayoutProfile->addWidget(pLabel);
        mWidgets << pLabel;

        if (i == 0)
            pLabel->setStyleSheet(labelStyle.arg("#B178FF"));
        else if (i == 5)
            pLabel->setStyleSheet(labelStyle.arg("#FF009B"));
    }

    // 특성 정보 추가
    addAbilityInfo(pProfile->getAbility());
}

void CharacterInfo::addSiblingSelector(const QList<Profile*> &siblings)
{
    // 보유 캐릭터 목록 추가
    QStringList characterList = {QString("보유 캐릭터\n(%1)").arg(siblings.size())};
    QString itemText = "[%1 Lv.%2]\n%3";

    for (const Profile *pSibling : siblings)
    {
        if (pSibling->getCharacterName() != mpCharacter->getProfile()->getCharacterName())
            characterList << itemText.arg(pSibling->getCharacterClass()).arg(pSibling->getItemLevel()).arg(pSibling->getCharacterName());
    }

    QComboBox *pComboBoxSibling = WidgetManager::createComboBox(characterList, 10, 200, 50);
    pComboBoxSibling->setFixedHeight(50);
    for (int i = 0; i < pComboBoxSibling->count(); i++)
        pComboBoxSibling->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
    ui->vLayoutProfile->addWidget(pComboBoxSibling);
    mWidgets << pComboBoxSibling;

    // 보유 캐릭터 목록에서 캐릭터 선택 시 캐릭터 변경
    connect(pComboBoxSibling, &QComboBox::currentIndexChanged, this, [&](int index){
        if (index != 0)
            emit CharacterSearch::getInstance()->searchRequested(mpCharacter->getSiblings().at(index - 1)->getCharacterName());
    });
}


void CharacterInfo::addAbilityInfo(const QHash<Ability, int> &abilities)
{
    QString text = "%1 : %2";

    addLayoutTitle("특성", ui->vLayoutProfile);

    for (int i = 0; i < static_cast<int>(Ability::size); i++)
    {
        Ability ability = static_cast<Ability>(i);
        QLabel *pLabelAbility = WidgetManager::createLabel(text.arg(abilityToQString(ability)).arg(abilities[ability]), 12);

        ui->vLayoutProfile->addWidget(pLabelAbility);
        mWidgets << pLabelAbility;
    }
}

void CharacterInfo::addEngraveInfo(const Engrave *pEngrave)
{
    EngraveManager *pEngraveManager = EngraveManager::getInstance();

    addLayoutTitle("각인", ui->vLayoutProfile);

    if (pEngrave == nullptr)
        return;

    const QList<QStringList> engraves = {pEngrave->getEngraves(), pEngrave->getPenalties()};

    // 증가 각인, 감소 각인 2회
    for (int i = 0; i < 2; i++)
    {
        QString textColor = "";
        if (i == 1)
            textColor = "red";

        // 각인 레벨별 레이아웃 생성
        QList<QVBoxLayout*> vLayouts(3, nullptr);

        for (int j = 2; j >= 0; j--)
        {
            QVBoxLayout* pVLayout = new QVBoxLayout();
            ui->vLayoutProfile->addLayout(pVLayout);
            vLayouts[j] = pVLayout;
            mLayouts << pVLayout;
        }

        // 각인 정보 추가
        for (const QString &engrave : engraves[i])
        {
            int level = 0;
            if (i == 0)
                level = pEngrave->getEngraveLevel(engrave);
            else if (i == 1)
                level = pEngrave->getPenaltyLevel(engrave);

            QHBoxLayout *pHLayout = new QHBoxLayout();
            vLayouts[level - 1]->addLayout(pHLayout);
            mLayouts << pHLayout;

            QLabel *pIconEngrave = WidgetManager::createIcon(pEngraveManager->iconPath(engrave), nullptr);
            pHLayout->addWidget(pIconEngrave);
            mWidgets << pIconEngrave;

            QLabel *pLabelEngraveLevel = WidgetManager::createLabel(QString("Lv.%1").arg(level), 10, textColor, 50);
            pHLayout->addWidget(pLabelEngraveLevel);
            mWidgets << pLabelEngraveLevel;
        }
    }
}

void CharacterInfo::addCardInfo(const Card *pCard)
{
    QString text = "%1 (%2각)";

    addLayoutTitle("카드", ui->vLayoutProfile);

    if (pCard == nullptr)
        return;

    const QList<QPair<QString, int>> &cardSets = pCard->cardSets();

    for (const QPair<QString, int> &cardSet : cardSets)
    {
        QLabel *pLabelCardSet = WidgetManager::createLabel(text.arg(cardSet.first).arg(cardSet.second), 10);
        ui->vLayoutProfile->addWidget(pLabelCardSet);
        mWidgets << pLabelCardSet;
    }
}

void CharacterInfo::initializeEquipLayout()
{
    ui->vLayoutEquip->setAlignment(Qt::AlignTop);

    addLayoutTitle("장비", ui->vLayoutEquip);

    // 방어구 정보 추가
    addArmorInfo(mpCharacter->getArmors());

    // 에스더 무기인 경우 무기의 세트효과는 장갑의 세트효과를 받음
    Weapon *pWeapon = mpCharacter->getWeapon();
    if (pWeapon != nullptr && pWeapon->itemGrade() == ItemGrade::에스더)
    {
        const Armor *pHand = mpCharacter->getArmor(ArmorPart::Hand);

        pWeapon->setItemSet(pHand->itemSet());
        pWeapon->setSetLevel(pHand->setLevel());
    }

    // 무기 정보 추가
    addWeaponInfo(mpCharacter->getWeapon());
    addHLine(ui->vLayoutEquip);

    // 엘릭서 정보 추가
    addElixirInfo(mpCharacter->getArmors());
}

void CharacterInfo::addArmorInfo(const QList<Armor *> &armors)
{
    for (int i = 0; i < 5; i++)
    {
        EquipInfo *pArmorInfo = new EquipInfo(armors[i]);

        ui->vLayoutEquip->addWidget(pArmorInfo);
        ui->vLayoutEquip->setAlignment(pArmorInfo, Qt::AlignLeft);
        mWidgets << pArmorInfo;
    }
}

void CharacterInfo::addWeaponInfo(const Weapon *pWeapon)
{
    EquipInfo *pWeaponInfo = new EquipInfo(pWeapon);

    ui->vLayoutEquip->addWidget(pWeaponInfo);
    ui->vLayoutEquip->setAlignment(pWeaponInfo, Qt::AlignLeft);
    mWidgets << pWeaponInfo;
}

void CharacterInfo::addElixirInfo(const QList<Armor *> &armors)
{
    int totalLevel = 0;
    QString elixirSetHead = "질서";
    QString elixirSetHand = "혼돈";

    for (const Armor *pArmor : armors)
    {
        const QList<Elixir> &elixirs = pArmor->elixirs();

        for (const Elixir &elixir : elixirs)
        {
            // 엘릭서 레벨 합계 계산
            if (pArmor->armorPart() == elixir.part || elixir.part == ArmorPart::size)
                totalLevel += elixir.level;

            // 투구, 장갑의 세트효과 추출
            if (pArmor->armorPart() == ArmorPart::Head && elixir.effect.contains("("))
                elixirSetHead = elixir.effect.sliced(0, 2);
            else if (pArmor->armorPart() == ArmorPart::Hand && elixir.effect.contains("("))
                elixirSetHand = elixir.effect.sliced(0, 2);
        }
    }

    addLayoutTitle("엘릭서 정보", ui->vLayoutEquip);

    // 엘릭서 레벨 정보 추가
    QLabel *pLabelTotalLevel = WidgetManager::createLabel(QString("연성 레벨 합 : %1").arg(totalLevel), 12);
    ui->vLayoutEquip->addWidget(pLabelTotalLevel);
    ui->vLayoutEquip->setAlignment(pLabelTotalLevel, Qt::AlignHCenter);
    mWidgets << pLabelTotalLevel;

    // 엘릭서 세트 활성화 정보 추가
    int enableLevel[] = {35, 40};

    if (elixirSetHead == elixirSetHand)
    {
        for (int i = 0; i < 2; i++)
        {
            if (totalLevel >= enableLevel[i])
            {
                QLabel *pLabelSetLevel = WidgetManager::createLabel(QString("%1 %2단계 활성화").arg(elixirSetHead).arg(i + 1), 12, "#36AF21");
                ui->vLayoutEquip->addWidget(pLabelSetLevel);
                ui->vLayoutEquip->setAlignment(pLabelSetLevel, Qt::AlignHCenter);
                mWidgets << pLabelSetLevel;
            }
        }
    }
}

void CharacterInfo::initializeAccessoryLayout()
{
    ui->vLayoutAccessory->setAlignment(Qt::AlignTop);

    addLayoutTitle("악세 / 스톤 / 팔찌", ui->vLayoutAccessory);

    // 악세서리 정보 추가
    addAccessoryInfo(mpCharacter->getAccessories());
    addHLine(ui->vLayoutAccessory);

    // 어빌리티 스톤 정보 추가
    addAbilityStoneInfo(mpCharacter->getAbilityStone());
    addHLine(ui->vLayoutAccessory);

    // 팔찌 정보 추가
    addBraceletInfo(mpCharacter->getBracelet());
}

void CharacterInfo::addAccessoryInfo(const QList<Accessory *> &accessories)
{
    for (const Accessory *pAccessory : accessories)
    {
        AccessoryInfo *pAccInfo = new AccessoryInfo(pAccessory);

        ui->vLayoutAccessory->addWidget(pAccInfo);
        ui->vLayoutAccessory->setAlignment(pAccInfo, Qt::AlignLeft);
        mWidgets << pAccInfo;
    }
}

void CharacterInfo::addAbilityStoneInfo(const AbilityStone *pAbilityStone)
{
    AbilityStoneInfo *pAbilityStoneInfo = new AbilityStoneInfo(pAbilityStone);

    ui->vLayoutAccessory->addWidget(pAbilityStoneInfo);
    ui->vLayoutAccessory->setAlignment(pAbilityStoneInfo, Qt::AlignLeft);
    mWidgets << pAbilityStoneInfo;
}

void CharacterInfo::addBraceletInfo(const Bracelet *pBracelet)
{
    BraceletInfo *pBraceletInfo = new BraceletInfo(pBracelet);

    ui->vLayoutAccessory->addWidget(pBraceletInfo);
    ui->vLayoutAccessory->setAlignment(pBraceletInfo, Qt::AlignLeft);
    mWidgets << pBraceletInfo;
}

void CharacterInfo::initializeGemLayout()
{
    ui->vLayoutGem1->setAlignment(Qt::AlignTop);
    ui->vLayoutGem2->setAlignment(Qt::AlignTop);

    addLayoutTitle("멸화 보석", ui->vLayoutGem1);
    addLayoutTitle("홍염 보석", ui->vLayoutGem2);

    addGemInfo(mpCharacter->getGems());
    addHLine(ui->vLayoutGem1);
    addHLine(ui->vLayoutGem2);

    addGemLevelAvgInfo(mpCharacter->getGems());
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

        if (pVLayout == nullptr)
        {
            delete pGemInfo;
            continue;
        }

        pVLayout->addWidget(pGemInfo);
        pVLayout->setAlignment(pGemInfo, Qt::AlignLeft);
        mWidgets << pGemInfo;
    }
}

void CharacterInfo::addGemLevelAvgInfo(const QList<Gem *> &gems)
{
    int levelSum[] = {0, 0};
    int count[] = {0, 0};

    // 보석 타입별 레벨 및 갯수 계산
    for (const Gem *pGem : gems)
    {
        if (pGem->gemType() == GemType::멸화)
        {
            count[0]++;
            levelSum[0] += pGem->gemLevel();
        }
        else if (pGem->gemType() == GemType::홍염)
        {
            count[1]++;
            levelSum[1] += pGem->gemLevel();
        }
    }

    // 보석 타입별 평균 레벨 정보 추가
    QList<QVBoxLayout*> layouts = {ui->vLayoutGem1, ui->vLayoutGem2};

    for (int i = 0; i < 2; i++)
    {
        QString typeStr;

        if (i == 0)
            typeStr = "멸화";
        else if (i == 1)
            typeStr = "홍염";

        addLayoutTitle(QString("%1 평균 레벨").arg(typeStr), layouts[i]);

        if (count[i] > 0)
        {
            QLabel *pLabelGemLevelAvg = WidgetManager::createLabel(QString::number(levelSum[i] / (double)count[i], 'f', 2), 12);
            layouts[i]->addWidget(pLabelGemLevelAvg);
            mWidgets << pLabelGemLevelAvg;
        }
    }
}

void CharacterInfo::initializeSkillLayout()
{
    ui->vLayoutSkill->setAlignment(Qt::AlignTop);

    const Profile *pProfile = mpCharacter->getProfile();
    const QString titleText = QString("스킬 (%1/%2)").arg(pProfile->getUsingSkillPoint()).arg(pProfile->getTotalSkillPoint());

    addLayoutTitle(titleText, ui->vLayoutSkill);

    addSkillInfo(mpCharacter->getSkills());
}

void CharacterInfo::addSkillInfo(const QList<Skill *> &skills)
{
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
            mWidgets << pSkillInfo;

            count++;
        }
    }
}

void CharacterInfo::addHLine(QLayout *pLayout)
{
    QFrame *pHLine = WidgetManager::createLine(QFrame::HLine);
    pLayout->addWidget(pHLine);
    mWidgets << pHLine;
}

void CharacterInfo::addLayoutTitle(const QString &title, QLayout *pLayout)
{
    QLabel *pLabelTitle = WidgetManager::createLabel(title, 12, "", 1000);
    pLabelTitle->setStyleSheet("QLabel { border-radius: 5px;"
                               "         padding: 2px;"
                               "         background-color: black; "
                               "         color: white; }");
    pLayout->addWidget(pLabelTitle);
    mWidgets << pLabelTitle;
}
