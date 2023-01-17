#include "profile_widget.h"
#include "ui_profile_widget.h"
#include "game_data/character/profile/profile.h"
#include "ui/widget_manager.h"
#include <QLabel>

ProfileWidget::ProfileWidget(QWidget* pParent, const Profile* pProfile) :
    QWidget(pParent),
    ui(new Ui::ProfileWidget),
    m_pProfile(pProfile)
{
    ui->setupUi(this);

    addLabels();
}

ProfileWidget::~ProfileWidget()
{
    for (QLabel* pLabel : m_labels)
        delete pLabel;
    delete ui;
}

void ProfileWidget::addLabels()
{
    // Group1 - title, name
    QLabel* pLabelTitle = WidgetManager::createLabel(m_pProfile->getTitle(), LABEL_WIDTH, LABEL_HEIGHT, 10, this);
    m_labels.append(pLabelTitle);
    ui->vLayoutGroup1->addWidget(pLabelTitle);

    QLabel* pLabelName = WidgetManager::createLabel(m_pProfile->getCharacterName(), LABEL_WIDTH, LABEL_HEIGHT, 10, this);
    m_labels.append(pLabelName);
    ui->vLayoutGroup1->addWidget(pLabelName);

    // Group2 - server, guild
    QLabel* pLabelServer = WidgetManager::createLabel(m_pProfile->getServer(), LABEL_WIDTH, LABEL_HEIGHT, 10, this);
    m_labels.append(pLabelServer);
    ui->vLayoutGroup2->addWidget(pLabelServer);

    QLabel* pLabelGuild = WidgetManager::createLabel(m_pProfile->getGuild(), LABEL_WIDTH, LABEL_HEIGHT, 10, this);
    m_labels.append(pLabelGuild);
    ui->vLayoutGroup2->addWidget(pLabelGuild);

    // Group3 - class, battleLv
    QLabel* pLabelClass = WidgetManager::createLabel(classToStr(m_pProfile->getClass()), LABEL_WIDTH, LABEL_HEIGHT, 10, this);
    m_labels.append(pLabelClass);
    ui->vLayoutGroup3->addWidget(pLabelClass);

    QLabel* pLabelBattleLv = WidgetManager::createLabel(QString("전투Lv. %1").arg(m_pProfile->getCharacterLevel()), LABEL_WIDTH, LABEL_HEIGHT, 10, this);
    m_labels.append(pLabelBattleLv);
    ui->vLayoutGroup3->addWidget(pLabelBattleLv);

    // Group4 - ExpLv, ItemLv
    QLabel* pLabelExpLv = WidgetManager::createLabel(QString("원정대Lv. %1").arg(m_pProfile->getExpLevel()), LABEL_WIDTH, LABEL_HEIGHT, 10, this);
    m_labels.append(pLabelExpLv);
    ui->vLayoutGroup4->addWidget(pLabelExpLv);

    QLabel* pLabelItemLv = WidgetManager::createLabel(QString("아이템Lv. %1").arg(m_pProfile->getItemLevel()), LABEL_WIDTH, LABEL_HEIGHT, 10, this);
    m_labels.append(pLabelItemLv);
    ui->vLayoutGroup4->addWidget(pLabelItemLv);

    // Ability
    QString abilityFormat = QString("%1 %2  %3 %4  %5 %6");
    const QMap<Ability, int>& abilities = m_pProfile->getAbilities();

    QString abilityText1 = abilityFormat.arg(abilityToStr(Ability::치명)).arg(abilities[Ability::치명])
                                        .arg(abilityToStr(Ability::특화)).arg(abilities[Ability::특화])
                                        .arg(abilityToStr(Ability::신속)).arg(abilities[Ability::신속]);
    QLabel* pLabelAbility1 = WidgetManager::createLabel(abilityText1, LABEL_WIDTH, LABEL_HEIGHT, 10, this);
    m_labels.append(pLabelAbility1);
    ui->hLayoutAbility1->addWidget(pLabelAbility1);

    QString abilityText2 = abilityFormat.arg(abilityToStr(Ability::제압)).arg(abilities[Ability::제압])
                                        .arg(abilityToStr(Ability::인내)).arg(abilities[Ability::인내])
                                        .arg(abilityToStr(Ability::숙련)).arg(abilities[Ability::숙련]);
    QLabel* pLabelAbility2 = WidgetManager::createLabel(abilityText2, LABEL_WIDTH, LABEL_HEIGHT, 10, this);
    m_labels.append(pLabelAbility2);
    ui->hLayoutAbility2->addWidget(pLabelAbility2);
}
