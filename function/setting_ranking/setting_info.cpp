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
    EngraveManager *pEngraveManager = EngraveManager::getInstance();
    QList<QHBoxLayout *> layouts = {ui->hLayoutEngrave3, ui->hLayoutEngrave2, ui->hLayoutEngrave1};

    QStringList engraveSummaries(3);

    for (int i = 0; i < engraveLevel.size(); i++)
    {
        int engraveCode = engrave.sliced(i * 3, 3).toInt();
        const QString &engraveName = pEngraveManager->getEngraveByCode(engraveCode);

        QVBoxLayout *pVLayout = new QVBoxLayout();
        layouts[engraveLevel[i].digitValue() - 1]->addLayout(pVLayout);

        QLabel *pEngraveIcon = WidgetManager::createIcon(pEngraveManager->iconPath(engraveName), nullptr);
        pVLayout->addWidget(pEngraveIcon);

        QLabel *pLabelLevel = WidgetManager::createLabel(engraveLevel[i], 10, "", 50);
        pVLayout->addWidget(pLabelLevel);

        engraveSummaries[engraveLevel[i].digitValue() - 1] += engraveName.front();
    }

    QString engraveSummary;

    for (int i = engraveSummaries.size() - 1; i >= 0; i--)
    {
        engraveSummary += engraveSummaries[i];
    }

    ui->groupEngrave->setFont(FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, 12));
    ui->groupEngrave->setTitle(QString("각인 (%1)").arg(engraveSummary));
}

QLabel *SettingInfo::createLabel(const QString &text, int fontSize, int width, const QString &style)
{
    QLabel *pLabel = WidgetManager::createLabel(text, fontSize);

    if (width != 0)
        pLabel->setFixedWidth(width);

    if (style != "")
        pLabel->setStyleSheet(style);

    return pLabel;
}
