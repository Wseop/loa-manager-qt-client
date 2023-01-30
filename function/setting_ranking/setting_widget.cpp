#include "setting_widget.h"
#include "ui_setting_widget.h"
#include "game_data/character/settingcode_builder.h"
#include "game_data/character/engrave/engrave_manager.h"
#include "game_data/character/profile/enum/ability.h"
#include "ui/widget_manager.h"
#include "ui/font_manager.h"
#include "function/setting_ranking/characterlist_widget.h"
#include <functional>
#include <QLabel>

SettingWidget::SettingWidget(QWidget* pParent, int index, const SettingCode& settingCode, QList<CharacterInfo> characterInfos, int total) :
    QWidget(pParent),
    ui(new Ui::SettingWidget),
    m_pCharacterListWidget(nullptr)
{
    ui->setupUi(this);

    setFonts();
    setAlignments();
    setRatio(index, characterInfos.size(), total);
    setAbility(settingCode);
    setSetEffect(settingCode);
    setEngrave(settingCode);
    setCharacterInfos(characterInfos);
}

SettingWidget::~SettingWidget()
{
    for (QLabel* pLabel : m_labels)
        delete pLabel;
    for (QHBoxLayout* pHLayout : m_hLayouts)
        delete pHLayout;
    for (QVBoxLayout* pVLayout : m_vLayouts)
        delete pVLayout;
    delete m_pCharacterListWidget;
    delete ui;
}

void SettingWidget::setFonts()
{
    FontManager* pFontManager = FontManager::getInstance();
    QFont nanumBold10 = pFontManager->getFont(FontFamily::NanumSquareNeoBold, 10);
    QFont nanumBold14 = pFontManager->getFont(FontFamily::NanumSquareNeoBold, 14);

    ui->pbRatio->setFont(nanumBold14);
    ui->groupAbility->setFont(nanumBold10);
    ui->groupAccessory->setFont(nanumBold10);
    ui->groupSetEffect->setFont(nanumBold10);
    ui->groupClassEngrave->setFont(nanumBold10);
    ui->groupEngrave->setFont(nanumBold10);
}

void SettingWidget::setAlignments()
{
    ui->vLayoutAbility->setAlignment(Qt::AlignHCenter);
    ui->vLayoutSetEffect->setAlignment(Qt::AlignHCenter);
}

void SettingWidget::setRatio(int index, int count, int total)
{
    double ratio = count / static_cast<double>(total) * 100;
    QString ratioText = QString("%1위) 채용률 %2% (%3 캐릭터)").arg(index).arg(ratio, 0, 'f', 2, QChar(' ' )).arg(count);
    ui->pbRatio->setText(ratioText);
}

void SettingWidget::setAbility(const SettingCode& settingCode)
{
    // 부위별 특성 가중치
    const QList<int> WEIGHTING = {5, 5, 3, 3, 2, 2};
    QList<Ability> abilities = SettingcodeBuilder::parseAbility(settingCode);

    // 특성별 가중치 계산 (내림차순 정렬)
    QList<QPair<Ability, int>> abilityWeighting;
    for (int i = 0; i < static_cast<int>(Ability::Size); i++)
        abilityWeighting.append({static_cast<Ability>(i), 0});
    for (int i = 0; i < abilities.size(); i++)
    {
        const Ability& ability = abilities[i];
        abilityWeighting[static_cast<int>(ability)].second += WEIGHTING[i];
    }
    std::sort(abilityWeighting.begin(), abilityWeighting.end(), [&](auto& a, auto& b){
        return a.second > b.second;
    });
    // 특성 label 추가 (가중치 0제외)
    QString abilityText;
    for (const auto& ability : abilityWeighting)
    {
        if (ability.second != 0)
            abilityText += abilityToStr(ability.first).at(0);
    }
    QLabel* pLabelAbility = WidgetManager::createLabel(abilityText, LABEL_WIDTH, LABEL_HEIGHT, 12, this);
    m_labels.append(pLabelAbility);
    ui->vLayoutAbility->addWidget(pLabelAbility);

    // 착용 악세 정보 추가
    const QStringList parts = {"목걸이", "귀걸이", "반지"};
    for (int i = 0; i < parts.size(); i++)
    {
        QHBoxLayout* pHLayout = new QHBoxLayout();
        m_hLayouts.append(pHLayout);
        ui->vLayoutAccessory->addLayout(pHLayout);

        QLabel* pLabelPart = WidgetManager::createLabel(parts[i], LABEL_WIDTH, LABEL_HEIGHT, 12, this);
        m_labels.append(pLabelPart);
        pHLayout->addWidget(pLabelPart);

        QString accAbilityText = QString("%1 %2").arg(abilityToStr(abilities[i * 2]), abilityToStr(abilities[(i * 2) + 1]));
        QLabel* pLabelAccAbility = WidgetManager::createLabel(accAbilityText, LABEL_WIDTH, LABEL_HEIGHT, 10, this);
        m_labels.append(pLabelAccAbility);
        pHLayout->addWidget(pLabelAccAbility);
    }
}

void SettingWidget::setSetEffect(const SettingCode& settingCode)
{
    const QList<SetEffect> setEffects = SettingcodeBuilder::parseSetEffect(settingCode);

    QList<int> setEffectCounts(static_cast<int>(SetEffect::Size), 0);
    for (const SetEffect& setEffect : setEffects)
    {
        setEffectCounts[static_cast<int>(setEffect)]++;
    }

    // 세트 효과 label 추가
    QString setEffectText;
    for (int i = 0; i < setEffectCounts.size(); i++)
    {
        const int& setEffectCount = setEffectCounts[i];
        if (setEffectCount == 0)
            continue;

        setEffectText += QString("%1%2 ").arg(setEffectCount).arg(setEffectToStr(static_cast<SetEffect>(i)));
    }
    setEffectText.chop(1);
    QLabel* pLabelSetEffect = WidgetManager::createLabel(setEffectText, LABEL_WIDTH, LABEL_HEIGHT, 12, this);
    m_labels.append(pLabelSetEffect);
    ui->vLayoutSetEffect->addWidget(pLabelSetEffect);
}

void SettingWidget::setEngrave(const SettingCode& settingCode)
{
    const int ICON_WIDTH = 50;
    const int ICON_HEIGHT = 50;
    EngraveManager* pEngraveManager = EngraveManager::getInstance();

    // index + 1 == engrave level
    QList<QList<PairEngraveValue>> classEngravesList = SettingcodeBuilder::parseClassEngrave(settingCode);
    QList<QList<PairEngraveValue>> normalEngravesList = SettingcodeBuilder::parseNormalEngrave(settingCode);

    // 직업 각인 정보 추가
    for (int i = 2; i >= 0; i--)
    {
        for (const PairEngraveValue& classEngrave : classEngravesList[i])
        {
            QLabel* pIcon = WidgetManager::createIcon(pEngraveManager->iconPath(classEngrave.first), nullptr, ICON_WIDTH, ICON_HEIGHT, this);
            m_labels.append(pIcon);
            ui->hLayoutClassEngrave->addWidget(pIcon);
        }
    }

    // 모든 각인 정보 추가
    for (int i = 2; i >= 0; i--)
    {
        for (const PairEngraveValue& normalEngrave : normalEngravesList[i])
        {
            QVBoxLayout* pVLayout = new QVBoxLayout();
            m_vLayouts.append(pVLayout);
            ui->hLayoutEngrave->addLayout(pVLayout);

            QLabel* pIcon = WidgetManager::createIcon(pEngraveManager->iconPath(normalEngrave.first), nullptr, ICON_WIDTH, ICON_HEIGHT, this);
            m_labels.append(pIcon);
            pVLayout->addWidget(pIcon);

            QLabel* pLabelLevel = WidgetManager::createLabel(QString::number(i + 1), 50, 25, 10, this);
            m_labels.append(pLabelLevel);
            pVLayout->addWidget(pLabelLevel);
        }
        for (const PairEngraveValue& classEngrave : classEngravesList[i])
        {
            QVBoxLayout* pVLayout = new QVBoxLayout();
            m_vLayouts.append(pVLayout);
            ui->hLayoutEngrave->addLayout(pVLayout);

            QLabel* pIcon = WidgetManager::createIcon(pEngraveManager->iconPath(classEngrave.first), nullptr, ICON_WIDTH, ICON_HEIGHT, this);
            m_labels.append(pIcon);
            pVLayout->addWidget(pIcon);

            QLabel* pLabelLevel = WidgetManager::createLabel(QString::number(i + 1), 50, 25, 10, this);
            m_labels.append(pLabelLevel);
            pVLayout->addWidget(pLabelLevel);
        }
    }
}

void SettingWidget::setCharacterInfos(QList<CharacterInfo>& characterInfos)
{
    m_pCharacterListWidget = new CharacterListWidget(nullptr, characterInfos);
    connect(ui->pbRatio, &QPushButton::released, this, [&](){
        m_pCharacterListWidget->show();
    });
}
