#include "setting_info.h"
#include "ui_setting_info.h"
#include "ui/widget_manager.h"
#include "ui/font_manager.h"
#include "game/engrave/engrave_manager.h"

#include <QLabel>
#include <QGroupBox>

SettingInfo::SettingInfo(CharacterSetting characterSetting, int rank, int count, int total) :
    ui(new Ui::SettingInfo)
{
    ui->setupUi(this);
    ui->hLayoutMain->setAlignment(Qt::AlignCenter);

    initializeLayoutRatio(rank, count, total);
    initializeLayoutInfo(characterSetting);
    initializeLayoutEngrave(characterSetting.engrave, characterSetting.engraveLevel);
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

void SettingInfo::initializeLayoutRatio(int rank, int count, int total)
{
    QLabel *pLabelRank = createLabel(QString("[%1위]").arg(rank), 12, 100,
                                     "QLabel { border-radius: 5px;"
                                     "         padding: 2px;"
                                     "         background-color: black; "
                                     "         color: white; }");
    ui->vLayoutRatio->addWidget(pLabelRank);

    QLabel *pLabelCount = createLabel(QString("%1 캐릭터").arg(count), 11, 100, "");
    ui->vLayoutRatio->addWidget(pLabelCount);

    double ratio = (count / static_cast<double>(total)) * 100;

    QLabel *pLabelRatio = createLabel(QString("(%1%)").arg(ratio, 0, 'f', 2), 11, 100, "");
    ui->vLayoutRatio->addWidget(pLabelRatio);
}

void SettingInfo::initializeLayoutInfo(const CharacterSetting &characterSetting)
{
    QLabel *pLabelItemSet = createLabel(characterSetting.itemSet, 12, 0,
                                        "QLabel { border-radius: 5px;"
                                        "         padding: 2px;"
                                        "         background-color: black; "
                                        "         color: #D7AC87; }");
    ui->hLayoutInfo->addWidget(pLabelItemSet);

    QLabel *pLabelClassEngrave = createLabel(extractClassEngrave(characterSetting.engrave, characterSetting.engraveLevel),
                                             12, 0,
                                             "QLabel { border-radius: 5px;"
                                             "         padding: 2px;"
                                             "         background-color: black; "
                                             "         color: #F99200; }");
    ui->hLayoutInfo->addWidget(pLabelClassEngrave);

    QLabel *pLabelAbility = createLabel(characterSetting.ability, 12, 0,
                                        "QLabel { border-radius: 5px;"
                                        "         padding: 2px;"
                                        "         background-color: black; "
                                        "         color: white; }");
    ui->hLayoutInfo->addWidget(pLabelAbility);

    QLabel *pLabelElixir = createLabel(characterSetting.elixir, 12, 0,
                                       "QLabel { border-radius: 5px;"
                                       "         padding: 2px;"
                                       "         background-color: black; "
                                       "         color: #00B700; }");
    ui->hLayoutInfo->addWidget(pLabelElixir);
}

void SettingInfo::initializeLayoutEngrave(const QString &engrave, const QString &engraveLevel)
{
    ui->groupEngrave->setFont(FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, 10));

    EngraveManager *pEngraveManager = EngraveManager::getInstance();

    QList<QHBoxLayout *> layouts = {ui->hLayoutEngrave3, ui->hLayoutEngrave2, ui->hLayoutEngrave1};

    for (int i = 0; i < engraveLevel.size(); i++)
    {
        int engraveCode = engrave.sliced(i * 3, 3).toInt();
        const QString &engraveName = pEngraveManager->getEngraveByCode(engraveCode);

        QVBoxLayout *pVLayout = new QVBoxLayout();
        layouts[engraveLevel[i].digitValue() - 1]->addLayout(pVLayout);
        mLayouts << pVLayout;

        QLabel *pEngraveIcon = WidgetManager::createIcon(pEngraveManager->iconPath(engraveName), nullptr);
        pVLayout->addWidget(pEngraveIcon);
        mWidgets << pEngraveIcon;

        QLabel *pLabelLevel = WidgetManager::createLabel(engraveLevel[i], 10, "", 50);
        pVLayout->addWidget(pLabelLevel);
        mWidgets << pLabelLevel;
    }
}

QLabel *SettingInfo::createLabel(const QString &text, int fontSize, int width, const QString &style)
{
    QLabel *pLabel = WidgetManager::createLabel(text, fontSize);

    if (width != 0)
        pLabel->setFixedWidth(width);

    if (style != "")
        pLabel->setStyleSheet(style);

    mWidgets << pLabel;

    return pLabel;
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
