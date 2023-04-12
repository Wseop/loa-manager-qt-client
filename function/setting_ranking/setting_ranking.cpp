#include "setting_ranking.h"
#include "ui_setting_ranking.h"
#include "ui/widget_manager.h"
#include "ui/font_manager.h"
#include "resource/resource_manager.h"
#include "api/api_manager.h"
#include "api/response_parser.h"
#include "game/engrave/engrave_manager.h"
#include "function/setting_ranking/setting_info.h"

#include <QLabel>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QProgressDialog>

#include <algorithm>

SettingRanking *SettingRanking::mpInstance = nullptr;

SettingRanking::SettingRanking() :
    ui(new Ui::SettingRanking),
    mTotalDataCount(0),
    mDataCounts(3, 0),
    mpClassSelector(nullptr),
    mpSearchButton(nullptr)
{
    ui->setupUi(this);

    mLines = {ui->line_2, ui->line_3, ui->line_4, ui->line_5, ui->line_6};

    mClassEngraveLayouts = {ui->vLayoutClassEngrave1, ui->vLayoutClassEngrave2, ui->vLayoutClassEngrave3};
    mItemSetLayouts = {ui->vLayoutItemSet1, ui->vLayoutItemSet2, ui->vLayoutItemSet3};
    mAbilityLayouts = {ui->vLayoutAbility1, ui->vLayoutAbility2, ui->vLayoutAbility3};
    mElixirLayouts = {ui->vLayoutElixir1, ui->vLayoutElixir2, ui->vLayoutElixir3};
    mEngraveLayouts = {ui->vLayoutEngrave1, ui->vLayoutEngrave2, ui->vLayoutEngrave3};
    mOverallLayouts = {ui->vLayoutOverall1, ui->vLayoutOverall2, ui->vLayoutOverall3};

    initializeInputLayout();
    initializeOutputLayout();
}

SettingRanking::~SettingRanking()
{
    delete ui;
}

void SettingRanking::initializeInputLayout()
{
    ui->hLayoutInput->setAlignment(Qt::AlignCenter);

    initializeClassSelector();
    initializeSearchButton();
    initializeHelp();
}

void SettingRanking::initializeClassSelector()
{
    QJsonArray classArray = ResourceManager::getInstance()->loadJson("class").find("Class")->toArray();
    QStringList classNames;

    for (const QJsonValue &value : classArray)
    {
        classNames << value.toObject().find("Child")->toVariant().toStringList();
    }

    QGroupBox *pGroupBox = WidgetManager::createGroupBox("직업 선택");
    ui->hLayoutInput->addWidget(pGroupBox);
    ui->hLayoutInput->setAlignment(pGroupBox, Qt::AlignCenter);
    mWidgets << pGroupBox;

    QHBoxLayout *pHLayout = new QHBoxLayout();
    pGroupBox->setLayout(pHLayout);
    mLayouts << pHLayout;

    mpClassSelector = WidgetManager::createComboBox(classNames);
    pHLayout->addWidget(mpClassSelector);
}

void SettingRanking::initializeSearchButton()
{
    mpSearchButton = WidgetManager::createPushButton("검색");

    connect(mpSearchButton, &QPushButton::released, this, [&]()
    {
        // 데이터 초기화
        mClassEngraveCodes.clear();
        mTotalDataCount = 0;
        mDataCounts = QList<int>(3, 0);
        for (QWidget *pWidget : mOutputs)
            delete pWidget;
        mOutputs.clear();

        mpSearchButton->setDisabled(true);
        ui->labelInfo->setText("데이터 불러오는 중...");

        // 직업 각인 목록 업데이트
        QStringList classEngraves = EngraveManager::getInstance()->getClassEngraves(mpClassSelector->currentText());

        for (const QString &classEngrave : classEngraves)
        {
            mClassEngraveCodes << EngraveManager::getInstance()->getEngraveCode(classEngrave);
        }

        // 데이터 로드
        QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();

        connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&](QNetworkReply *pReply)
        {
            // 데이터 parsing
            QList<CharacterSetting> characterSettings =
                    ResponseParser::parseCharacterSettings(QJsonDocument::fromJson(pReply->readAll()));

            // 데이터 필터링
            characterSettings = filterCharacterSettings(characterSettings);
            mTotalDataCount = characterSettings.size();

            ui->labelInfo->setText(QString("검색된 캐릭터 수 : %1").arg(mTotalDataCount));

            for (int i = 0; i < mLines.size(); i++)
            {
                mLines[i]->show();
            }

            // 각인 코드 오름차순 정렬
            for (CharacterSetting &characterSetting : characterSettings)
            {
                sortEngraveCodes(characterSetting.engrave, characterSetting.engraveLevel);
            }

            // 직업 각인별로 데이터 분류 및 직업 각인별 사용률 출력
            QList<QList<CharacterSetting>> classifiedByClassEngrave = classifyByClassEngrave(characterSettings);
            showClassEngraveRatio();

            // 세트효과로 분류 및 결과 출력
            for (int i = 0; i < 3; i++)
            {
                QList<QPair<QString, int>> classifiedByItemSet = classifyByItemSet(classifiedByClassEngrave[i]);
                showRatio("#D7AC87", "[세트 효과]", i, mItemSetLayouts, classifiedByItemSet);
            }

            QCoreApplication::processEvents();

            // 특성으로 분류 및 결과 출력
            for (int i = 0; i < 3; i++)
            {
                QList<QPair<QString, int>> classifiedByAbility = classifyByAbility(classifiedByClassEngrave[i]);
                showRatio("white", "[특성]", i, mAbilityLayouts, classifiedByAbility);
            }

            QCoreApplication::processEvents();

            // 엘릭서로 분류 및 결과 출력
            for (int i = 0; i < 3; i++)
            {
                QList<QPair<QString, int>> classifiedByElixir = classifyByElixir(classifiedByClassEngrave[i]);
                showRatio("#00B700", "[엘릭서]", i, mElixirLayouts, classifiedByElixir);
            }

            QCoreApplication::processEvents();

            // 각인으로 분류 및 결과 출력
            for (int i = 0; i < 3; i++)
            {
                QList<QPair<QString, int>> classifiedByEngrave = classifyByEngrave(classifiedByClassEngrave[i]);
                showEngraveRatio(i, classifiedByEngrave);
            }

            QCoreApplication::processEvents();

            // 모든 항목을 종합한 정보로 분류 및 결과 출력
            QProgressDialog progress("종합 비율 처리중", QString(), 0, 3, nullptr);
            progress.setWindowModality(Qt::WindowModal);

            for (int i = 0; i < 3; i++)
            {
                QList<QPair<QString, int>> classifiedOverall = classifyOverall(classifiedByClassEngrave[i]);
                showOverallRatio(i, classifiedOverall);

                progress.setValue(i);
                QCoreApplication::processEvents();
            }

            progress.setValue(3);

            mpSearchButton->setEnabled(true);
        });
        connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

        ApiManager::getInstance()->get(pNetworkManager, ApiType::LoaManager,
                                       static_cast<int>(LoamanagerApi::GetCharacterSetting),
                                       mpClassSelector->currentText(), "");
    });

    ui->hLayoutInput->addWidget(mpSearchButton);
    ui->hLayoutInput->setAlignment(mpSearchButton, Qt::AlignCenter);
    mWidgets << mpSearchButton;
}

void SettingRanking::initializeHelp()
{
    QString help = "[검색 조건]\n1620이상, 엘릭서 세트효과 활성화\n배신세트 제외\n특성 제인숙 제외";
    QLabel *pLabelHelp = WidgetManager::createLabel(help, 10, "", 300, 75);

    ui->hLayoutInput->addWidget(pLabelHelp);
    mWidgets << pLabelHelp;
}

void SettingRanking::initializeOutputLayout()
{
    for (int i = 0; i < mLines.size(); i++)
    {
        mLines[i]->hide();
    }

    for (int i = 0; i < 3; i++)
    {
        mClassEngraveLayouts[i]->setAlignment(Qt::AlignTop);
        mItemSetLayouts[i]->setAlignment(Qt::AlignTop);
        mAbilityLayouts[i]->setAlignment(Qt::AlignTop);
        mElixirLayouts[i]->setAlignment(Qt::AlignTop);
        mEngraveLayouts[i]->setAlignment(Qt::AlignTop);
        mOverallLayouts[i]->setAlignment(Qt::AlignTop);
    }

    ui->labelInfo->setFont(FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, 16));
}

QList<CharacterSetting> SettingRanking::filterCharacterSettings(const QList<CharacterSetting> &characterSettings)
{
    QList<CharacterSetting> filteredCharacterSettings;

    static QRegularExpression regExpAbilityFilter("제|인|숙");

    for (int i = 0; i < characterSettings.size(); i++)
    {
        const CharacterSetting &characterSetting = characterSettings[i];

        if (characterSetting.itemSet.contains("배신") ||
            characterSetting.engraveLevel.size() < 5 ||
            characterSetting.ability.contains(regExpAbilityFilter) ||
            characterSetting.elixir == "")
            continue;

        filteredCharacterSettings << characterSetting;
    }

    return filteredCharacterSettings;
}

void SettingRanking::sortEngraveCodes(QString &engrave, QString &engraveLevel)
{
    QList<int> engraveCodes;
    QHash<int, QString> engraveLevelMap;

    for (int i = 0; i < engraveLevel.size(); i++)
    {
        int engraveCode = engrave.sliced(i * 3, 3).toInt();

        engraveCodes << engraveCode;
        engraveLevelMap[engraveCode] = engraveLevel[i];
    }

    std::sort(engraveCodes.begin(), engraveCodes.end());

    QString sortedEngrave;
    QString sortedEngraveLevel;

    for (const int &engraveCode : engraveCodes)
    {
        sortedEngrave += QString::number(engraveCode);
        sortedEngraveLevel += engraveLevelMap[engraveCode];
    }

    engrave = sortedEngrave;
    engraveLevel = sortedEngraveLevel;
}

QList<QList<CharacterSetting>> SettingRanking::classifyByClassEngrave(const QList<CharacterSetting> &characterSettings)
{
    QList<QList<CharacterSetting>> classifiedByClassEngrave(3);

    for (int i = 0; i < characterSettings.size(); i++)
    {
        const CharacterSetting &characterSetting = characterSettings[i];
        const QString &engrave = characterSetting.engrave;

        int classifyIndex = -1;
        int matchCount = 0;

        for (int i = 0; i < characterSetting.engraveLevel.size(); i++)
        {
            int engraveCode = engrave.sliced(i * 3, 3).toInt();
            int index = mClassEngraveCodes.indexOf(engraveCode);

            if (index != -1)
            {
                matchCount++;
                classifyIndex = index;
            }
        }

        if (matchCount == 1)
        {
            classifiedByClassEngrave[classifyIndex] << characterSetting;
            mDataCounts[classifyIndex]++;
        }
        else if (matchCount == 2)
        {
            classifiedByClassEngrave[2] << characterSetting;
            mDataCounts[2]++;
        }
    }

    return classifiedByClassEngrave;
}

QList<QPair<QString, int> > SettingRanking::classifyByItemSet(const QList<CharacterSetting> &characterSettings)
{
    QHash<QString, int> itemSetCount;

    for (const CharacterSetting &characterSetting : characterSettings)
    {
        itemSetCount[characterSetting.itemSet]++;
    }

    QList<QPair<QString, int>> itemSetCounts;

    for (auto iter = itemSetCount.begin(); iter != itemSetCount.end(); iter++)
    {
        itemSetCounts.append({iter.key(), iter.value()});
    }

    std::sort(itemSetCounts.begin(), itemSetCounts.end(), [&](auto &a, auto &b)
    {
        return a.second > b.second;
    });

    return itemSetCounts;
}

QList<QPair<QString, int> > SettingRanking::classifyByAbility(const QList<CharacterSetting> &characterSettings)
{
    QHash<QString, int> abilityCount;

    for (const CharacterSetting &characterSetting : characterSettings)
    {
        abilityCount[characterSetting.ability]++;
    }

    QList<QPair<QString, int>> abilityCounts;

    for (auto iter = abilityCount.begin(); iter != abilityCount.end(); iter++)
    {
        abilityCounts.append({iter.key(), iter.value()});
    }

    std::sort(abilityCounts.begin(), abilityCounts.end(), [&](auto &a, auto &b)
    {
        return a.second > b.second;
    });

    return abilityCounts;
}

QList<QPair<QString, int> > SettingRanking::classifyByElixir(const QList<CharacterSetting> &characterSettings)
{
    QHash<QString, int> elixirCount;

    for (const CharacterSetting &characterSetting : characterSettings)
    {
        elixirCount[characterSetting.elixir]++;
    }

    QList<QPair<QString, int>> elixirCounts;

    for (auto iter = elixirCount.begin(); iter != elixirCount.end(); iter++)
    {
        elixirCounts.append({iter.key(), iter.value()});
    }

    std::sort(elixirCounts.begin(), elixirCounts.end(), [&](auto &a, auto &b)
    {
        return a.second > b.second;
    });

    return elixirCounts;
}

QList<QPair<QString, int> > SettingRanking::classifyByEngrave(const QList<CharacterSetting> &characterSettings)
{
    QHash<QString, int> engraveCount;

    for (const CharacterSetting &characterSetting : characterSettings)
    {
        QString key = characterSetting.engrave + "," + characterSetting.engraveLevel;

        engraveCount[key]++;
    }

    QList<QPair<QString, int>> engraveCounts;

    for (auto iter = engraveCount.begin(); iter != engraveCount.end(); iter++)
    {
        engraveCounts.append({iter.key(), iter.value()});
    }

    std::sort(engraveCounts.begin(), engraveCounts.end(), [&](auto &a, auto &b)
    {
        return a.second > b.second;
    });

    return engraveCounts;
}

QList<QPair<QString, int> > SettingRanking::classifyOverall(const QList<CharacterSetting> &characterSettings)
{
    QHash<QString, int> settingCount;

    for (const CharacterSetting &characterSetting : characterSettings)
    {
        QString setting = "";

        setting += characterSetting.itemSet + ",";
        setting += characterSetting.ability + ",";
        setting += characterSetting.elixir + ",";
        setting += characterSetting.engrave + ",";
        setting += characterSetting.engraveLevel;

        settingCount[setting]++;
    }

    QList<QPair<QString, int>> settingCounts;

    for (auto iter = settingCount.begin(); iter != settingCount.end(); iter++)
    {
        settingCounts.append({iter.key(), iter.value()});
    }

    std::sort(settingCounts.begin(), settingCounts.end(), [&](auto &a, auto &b)
    {
        return a.second > b.second;
    });

    return settingCounts;
}

void SettingRanking::showClassEngraveRatio()
{
    static const QString labelStyle = "QLabel { border-radius: 5px;"
                                      "         padding: 2px;"
                                      "         background-color: black; "
                                      "         color: #E4BA27; }";

    for (int i = 0; i < 3; i++)
    {
        QString classEngrave = i == 2 ? "쌍직각" :
                                        EngraveManager::getInstance()->getEngraveByCode(mClassEngraveCodes[i]);
        double usageRate = (mDataCounts[i] / static_cast<double>(mTotalDataCount)) * 100;

        QLabel *pLabelUsageRate = WidgetManager::createLabel(QString("%1 (%2%)").arg(classEngrave).arg(usageRate, 0, 'f', 2),
                                                             16, "", 500, 50);
        pLabelUsageRate->setStyleSheet(labelStyle);
        mClassEngraveLayouts[i]->addWidget(pLabelUsageRate);
        mOutputs << pLabelUsageRate;
    }
}

void SettingRanking::showRatio(QString textColor, QString title, int index, const QList<QVBoxLayout *> &layouts, const QList<QPair<QString, int> > &classifiedData)
{
    const QString labelStyle = QString("QLabel { border-radius: 5px;"
                                              "         padding: 2px;"
                                              "         background-color: black; "
                                              "         color: %1; }").arg(textColor);

    QLabel *pLabelTitle = WidgetManager::createLabel(title, 16, "", 500);
    layouts[index]->addWidget(pLabelTitle);
    mOutputs << pLabelTitle;

    for (int i = 0; i < classifiedData.size(); i++)
    {
        double usageRate = (classifiedData[i].second / static_cast<double>(mDataCounts[index])) * 100;

        QLabel *pLabelUsageRate = WidgetManager::createLabel(QString("%1 (%2%)")
                                                           .arg(classifiedData[i].first)
                                                           .arg(usageRate, 0, 'f', 2),
                                                           14, "", 500);
        pLabelUsageRate->setStyleSheet(labelStyle);
        layouts[index]->addWidget(pLabelUsageRate);
        mOutputs << pLabelUsageRate;
    }
}

void SettingRanking::showEngraveRatio(int index, const QList<QPair<QString, int> > &engraveRatio)
{
    static const QString labelStyle = "QLabel { border-radius: 5px;"
                                      "         padding: 2px;"
                                      "         background-color: black; "
                                      "         color: #FFFFAC; }";

    QLabel *pLabelTitle = WidgetManager::createLabel("[각인]", 16, "", 500);
    mEngraveLayouts[index]->addWidget(pLabelTitle);
    mOutputs << pLabelTitle;

    static EngraveManager *pEngraveManager = EngraveManager::getInstance();

    for (int i = 0; i < engraveRatio.size(); i++)
    {
        if (i == 10)
            break;

        QLabel *pLabelUsageRate = WidgetManager::createLabel("", 14, "", 500);
        pLabelUsageRate->setStyleSheet(labelStyle);
        mEngraveLayouts[index]->addWidget(pLabelUsageRate);
        mOutputs << pLabelUsageRate;

        // 각인 및 레벨 출력
        QStringList engraveSetting = engraveRatio[i].first.split(',');
        QStringList engraveSummaries(3, "");

        for (int level = 3; level > 0; level--)
        {
            for (int j = 0; j < engraveSetting[1].size(); j++)
            {
                const QString &engrave = pEngraveManager->getEngraveByCode(engraveSetting[0].sliced(j * 3, 3).toInt());
                int engraveLevel = engraveSetting[1][j].digitValue();

                if (engraveLevel != level)
                    continue;

                QLabel *pLabelEngrave = WidgetManager::createLabel(QString("%1 Lv.%2").arg(engrave).arg(engraveLevel), 10, "", 500);
                mEngraveLayouts[index]->addWidget(pLabelEngrave);
                mOutputs << pLabelEngrave;

                engraveSummaries[engraveLevel - 1] += engrave.front();
            }
        }

        double usageRate = (engraveRatio[i].second / static_cast<double>(mDataCounts[index])) * 100;

        // 각인 조합 앞글자만 추출하여 출력
        QString engraveSummary;

        for (int i = 2; i >= 0; i--)
        {
            engraveSummary += engraveSummaries[i];
        }

        pLabelUsageRate->setText(QString("%1 (%2%)").arg(engraveSummary).arg(usageRate, 0, 'f', 2));
    }
}

void SettingRanking::showOverallRatio(int index, const QList<QPair<QString, int> > &overallRatio)
{
    QLabel *pLabelTitle = WidgetManager::createLabel("[종합 비율]", 16, "", 500);
    mOverallLayouts[index]->addWidget(pLabelTitle);
    mOverallLayouts[index]->setAlignment(pLabelTitle, Qt::AlignHCenter);
    mOutputs << pLabelTitle;

    for (int i = 0; i < overallRatio.size(); i++)
    {
        QFrame *pHLine = WidgetManager::createLine(QFrame::HLine);
        mOverallLayouts[index]->addWidget(pHLine);
        mOutputs << pHLine;

        const QStringList settings = overallRatio[i].first.split(",");

        CharacterSetting characterSetting;
        characterSetting.itemSet = settings[0];
        characterSetting.ability = settings[1];
        characterSetting.elixir = settings[2];
        characterSetting.engrave = settings[3];
        characterSetting.engraveLevel = settings[4];

        SettingInfo *pSettingInfo = new SettingInfo(characterSetting, i + 1, overallRatio[i].second, mDataCounts[index]);
        mOverallLayouts[index]->addWidget(pSettingInfo);
        mOutputs << pSettingInfo;
    }
}

void SettingRanking::start()
{

}

SettingRanking *SettingRanking::getInstance()
{
    if (mpInstance == nullptr)
        mpInstance = new SettingRanking();

    return mpInstance;
}

void SettingRanking::destroyInstance()
{
    if (mpInstance == nullptr)
        return;

    delete mpInstance;
    mpInstance = nullptr;
}
