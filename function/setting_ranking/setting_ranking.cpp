#include "setting_ranking.h"
#include "ui_setting_ranking.h"
#include "ui/widget_manager.h"
#include "resource/resource_manager.h"
#include "api/api_manager.h"
#include "api/response_parser.h"
#include "function/setting_ranking/setting_info.h"

#include <QLabel>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <algorithm>

SettingRanking *SettingRanking::mpInstance = nullptr;

SettingRanking::SettingRanking() :
    ui(new Ui::SettingRanking),
    mNumOfCharacterSettings(0),
    mpClassSelector(nullptr),
    mpInfo(nullptr)
{
    ui->setupUi(this);

    initializeInputLayout();
}

SettingRanking::~SettingRanking()
{
    delete mpClassSelector;
    mpClassSelector = nullptr;

    delete mpInfo;
    mpInfo = nullptr;

    for (QWidget *pWidget : mOutput)
        delete pWidget;
    mOutput.clear();

    for (QWidget *pWidget : mWidgets)
        delete pWidget;
    mWidgets.clear();

    for (auto rIter = mLayouts.rbegin(); rIter != mLayouts.rend(); rIter++)
        delete *rIter;
    mLayouts.clear();

    delete ui;
}

void SettingRanking::initializeInputLayout()
{
    ui->hLayoutInput->setAlignment(Qt::AlignCenter);

    initializeClassSelector();
    initializeSearchButton();
    initializeInfo();
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
    QPushButton *pSearchButton = WidgetManager::createPushButton("검색");

    connect(pSearchButton, &QPushButton::released, this, [&]()
    {
        QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();

        connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&](QNetworkReply *pReply)
        {
            mappingCharacterSettings(ResponseParser::parseCharacterSettings(QJsonDocument::fromJson(pReply->readAll())));
            renderCharacterSettings();
        });
        connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

        ApiManager::getInstance()->get(pNetworkManager, ApiType::LoaManager,
                                       static_cast<int>(LoamanagerApi::GetCharacterSetting),
                                       mpClassSelector->currentText());
    });

    ui->hLayoutInput->addWidget(pSearchButton);
    ui->hLayoutInput->setAlignment(pSearchButton, Qt::AlignCenter);
    mWidgets << pSearchButton;
}

void SettingRanking::initializeInfo()
{
    mpInfo = WidgetManager::createLabel("", 12, "", 300, 50);
    ui->hLayoutInput->addWidget(mpInfo);

    QString help = "아이템 레벨 1620이상\n레이드 세팅인 캐릭터만 검색됩니다.";
    QLabel *pLabelHelp = WidgetManager::createLabel(help, 10, "", 300, 50);

    ui->hLayoutInput->addWidget(pLabelHelp);
    mWidgets << pLabelHelp;
}

void SettingRanking::mappingCharacterSettings(QList<CharacterSetting> characterSettings)
{
    // 기존 데이터 초기화
    mCharacterSettings.clear();
    mSettingCounts.clear();
    mNumOfCharacterSettings = 0;
    for (QWidget *pWidget : mOutput)
        delete pWidget;
    mOutput.clear();

    mpInfo->setText("데이터 처리 중...");

    // mapping
    for (CharacterSetting &characterSetting : characterSettings)
    {
        if (characterSetting.elixir == "" ||
            characterSetting.itemSet.contains("배신") ||
            characterSetting.engraveLevel.size() < 5 ||
            characterSetting.ability.contains("제") ||
            characterSetting.ability.contains("인") ||
            characterSetting.ability.contains("숙"))
            continue;

        sortEngraveSetting(characterSetting.engrave, characterSetting.engraveLevel);

        QString settingId;

        settingId += characterSetting.itemSet;
        settingId += characterSetting.engrave;
        settingId += characterSetting.engraveLevel;
        settingId += characterSetting.ability;
        settingId += characterSetting.elixir;

        mCharacterSettings[settingId] << characterSetting;
        mNumOfCharacterSettings++;
    }

    // 각 settingId에 해당하는 세팅의 갯수 계산 및 내림차순 정렬
    for (auto iter = mCharacterSettings.begin(); iter != mCharacterSettings.end(); iter++)
    {
        QString settingId = iter.key();
        int settingCount = iter.value().size();

        mSettingCounts.append({settingId, settingCount});
    }

    std::sort(mSettingCounts.begin(), mSettingCounts.end(), [&](const QPair<QString, int> &a, const QPair<QString, int> &b)
    {
        return a.second > b.second;
    });
}

void SettingRanking::sortEngraveSetting(QString &engrave, QString &engraveLevel)
{
    // 각인 세팅을 각인코드 기준 오름차순으로 정렬
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

void SettingRanking::renderCharacterSettings()
{
    mpInfo->setText(QString("검색된 캐릭터 수\n%1").arg(mNumOfCharacterSettings));

    for (int i = 0; i < mSettingCounts.size(); i++)
    {
        // 최대 100개까지만 rendering
        if (i == 100)
            break;

        QFrame *pHLine = WidgetManager::createLine(QFrame::HLine);

        ui->vLayoutOutput->addWidget(pHLine);
        mOutput << pHLine;

        const auto &settingCount = mSettingCounts[i];
        const QString &settingId = settingCount.first;

        SettingInfo *pSettingInfo = new SettingInfo(mCharacterSettings[settingId].first(),
                                                    settingCount.second,
                                                    mNumOfCharacterSettings);

        ui->vLayoutOutput->addWidget(pSettingInfo);
        mOutput << pSettingInfo;
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
