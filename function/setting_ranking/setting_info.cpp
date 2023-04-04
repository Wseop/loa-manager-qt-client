#include "setting_info.h"
#include "ui_setting_info.h"
#include "ui/widget_manager.h"
#include "game/engrave/engrave_manager.h"

#include <QLabel>
#include <QGroupBox>

SettingInfo::SettingInfo(CharacterSetting characterSetting, int count, int total) :
    ui(new Ui::SettingInfo),
    mCharacterSetting(characterSetting)
{
    ui->setupUi(this);

    initializeTitleLayout(count, total);
    initializeInfoLayout();
}

SettingInfo::~SettingInfo()
{
    for (QWidget *pWidget : mWidgets)
        delete pWidget;
    mWidgets.clear();

    for (auto rIter = mLayouts.rbegin(); rIter != mLayouts.rend(); rIter++)
        delete *rIter;
    mLayouts.clear();

    delete ui;
}

void SettingInfo::initializeTitleLayout(int count, int total)
{
    ui->hLayoutTitle->setAlignment(Qt::AlignCenter);

    addTitleInfo(mCharacterSetting.itemSet, extractClassEngrave(mCharacterSetting.engrave, mCharacterSetting.engraveLevel));
    addRatioInfo(count, total);
}

void SettingInfo::initializeInfoLayout()
{
    ui->hLayoutInfo->setAlignment(Qt::AlignCenter);

    addAbilityInfo(mCharacterSetting.ability);
    addEngraveInfo(mCharacterSetting.engrave, mCharacterSetting.engraveLevel);
    addElixirInfo(mCharacterSetting.elixir);
}

QString SettingInfo::extractClassEngrave(const QString &engrave, const QString &engraveLevel)
{
    EngraveManager *pEngraveManager = EngraveManager::getInstance();
    QString classEngrave = "";

    for (int i = 0; i < engraveLevel.size(); i++)
    {
        int engraveCode = engrave.sliced(i * 3, 3).toInt();
        const QString &engraveName = pEngraveManager->getEngraveByCode(engraveCode);

        if (pEngraveManager->isClassEngrave(engraveName))
        {
            if (classEngrave == "")
            {
                classEngrave = engraveName;
            }
            else
            {
                classEngrave += "|" + engraveName;
            }
        }
    }

    return classEngrave;
}

void SettingInfo::addTitleInfo(const QString &itemSet, const QString &classEngrave)
{
    QString title = QString("%1 %2").arg(itemSet, classEngrave);
    QLabel *pLabelTitle = WidgetManager::createLabel(title, 14, "", 500);

    pLabelTitle->setStyleSheet("QLabel { border-radius: 5px;"
                               "         padding: 2px;"
                               "         background-color: black;"
                               "         color: white; }");
    ui->hLayoutTitle->addWidget(pLabelTitle);
    mWidgets << pLabelTitle;
}

void SettingInfo::addRatioInfo(int count, int total)
{
    QString text = QString("%1캐릭터 (%2%)").arg(count).arg((count / (double)total) * 100, 0, 'f', 2);
    QLabel *pLabelRatio = WidgetManager::createLabel(text, 14);

    pLabelRatio->setStyleSheet("QLabel { border-radius: 5px;"
                               "         padding: 2px;"
                               "         background-color: black;"
                               "         color: white; }");
    ui->hLayoutTitle->addWidget(pLabelRatio);
    mWidgets << pLabelRatio;
}

void SettingInfo::addAbilityInfo(const QString &ability)
{
    QHBoxLayout *pHLayout = createGroupBox("특성");
    QLabel *pLabelAbility = WidgetManager::createLabel(ability, 12);

    pHLayout->addWidget(pLabelAbility);
    mWidgets << pLabelAbility;
}

void SettingInfo::addEngraveInfo(const QString &engrave, const QString &engraveLevel)
{
    QHBoxLayout *pEngraveLayout = createGroupBox("각인");
    QList<QHBoxLayout *> hLayouts(3, nullptr);

    for (int i = 2; i >= 0; i--)
    {
        QHBoxLayout *pHLayout = new QHBoxLayout();

        pEngraveLayout->addLayout(pHLayout);
        hLayouts[i] = pHLayout;
        mLayouts << pHLayout;
    }

    EngraveManager *pEngraveManager = EngraveManager::getInstance();

    for (int i = 0; i < engraveLevel.size(); i++)
    {
        const QString &engraveName = pEngraveManager->getEngraveByCode(engrave.sliced(i * 3, 3).toInt());
        int level = engraveLevel[i].digitValue();

        QVBoxLayout *pVLayout = new QVBoxLayout();

        hLayouts[level - 1]->addLayout(pVLayout);
        mLayouts << pVLayout;

        QLabel *pEngraveIcon = WidgetManager::createIcon(pEngraveManager->iconPath(engraveName), nullptr);

        pVLayout->addWidget(pEngraveIcon);
        mWidgets << pEngraveIcon;

        QLabel *pLabelLevel = WidgetManager::createLabel(QString::number(level), 10, "", 50);

        pVLayout->addWidget(pLabelLevel);
        mWidgets << pLabelLevel;
    }
}

void SettingInfo::addElixirInfo(const QString &elixir)
{
    QHBoxLayout *pHLayout = createGroupBox("엘릭서 세트 효과");
    QLabel *pLabelElixir = WidgetManager::createLabel(elixir, 12);

    pHLayout->addWidget(pLabelElixir);
    mWidgets << pLabelElixir;
}

QHBoxLayout *SettingInfo::createGroupBox(const QString &title)
{
    QGroupBox *pGroupBox = WidgetManager::createGroupBox(title, 14);

    ui->hLayoutInfo->addWidget(pGroupBox);
    //mWidgets << pGroupBox;

    QHBoxLayout *pHLayout = new QHBoxLayout();

    pGroupBox->setLayout(pHLayout);
    mLayouts << pHLayout;

    return pHLayout;
}
