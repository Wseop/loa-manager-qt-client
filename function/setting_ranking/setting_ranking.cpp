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
    mCharacterSettings(3),
    mSettingCounts(3),
    mpClassSelector(nullptr),
    mpSearchButton(nullptr)
{
    ui->setupUi(this);

    initializeInputLayout();
    initializeOutputLayout();
}

SettingRanking::~SettingRanking()
{
    delete mpClassSelector;
    mpClassSelector = nullptr;

    delete mpSearchButton;
    mpSearchButton = nullptr;

    for (QWidget *pWidget : mOutputs)
        delete pWidget;
    mOutputs.clear();

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
        mDataCounts = QList<int>(3);
        mCharacterSettings = QList<QHash<QString, QList<CharacterSetting>>>(3);
        mSettingCounts = QList<QList<QPair<QString, int>>>(3);

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
            // 불러온 데이터 처리
            QList<CharacterSetting> characterSettings =
                    ResponseParser::parseCharacterSettings(QJsonDocument::fromJson(pReply->readAll()));

            // 데이터 필터링
            characterSettings = filterCharacterSettings(characterSettings);
            mTotalDataCount = characterSettings.size();

            // 각인 세팅 정렬
            for (CharacterSetting &characterSetting : characterSettings)
            {
                sortEngraveSetting(characterSetting.engrave, characterSetting.engraveLevel);
            }

            // 직업 각인별로 데이터 분류
            QList<QList<CharacterSetting>> classifiedByClassEngrave = classifyByClassEngrave(characterSettings);

            // 세팅 ID를 부여하여 최종 분류
            classifyBySetting(classifiedByClassEngrave);

            // 결과 출력
            showCharacterSettings();
        });
        connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

        ApiManager::getInstance()->get(pNetworkManager, ApiType::LoaManager,
                                       static_cast<int>(LoamanagerApi::GetCharacterSetting),
                                       mpClassSelector->currentText());
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
    ui->vLayoutOutput1->setAlignment(Qt::AlignTop);
    ui->vLayoutOutput2->setAlignment(Qt::AlignTop);
    ui->vLayoutOutput3->setAlignment(Qt::AlignTop);

    ui->labelInfo->setFont(FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, 16));
}

QList<CharacterSetting> SettingRanking::filterCharacterSettings(const QList<CharacterSetting> &characterSettings)
{
    QList<CharacterSetting> filteredCharacterSettings;

    static QRegularExpression regExpAbilityFilter("제|인|숙");

    QProgressDialog progress("데이터 필터링중...", QString(), 0, characterSettings.size(), nullptr);
    progress.setWindowModality(Qt::WindowModal);

    for (int i = 0; i < characterSettings.size(); i++)
    {
        progress.setValue(i);
        QCoreApplication::processEvents();

        const CharacterSetting &characterSetting = characterSettings[i];

        if (characterSetting.itemSet.contains("배신") ||
            characterSetting.engraveLevel.size() < 5 ||
            characterSetting.ability.contains(regExpAbilityFilter) ||
            characterSetting.elixir == "")
            continue;

        filteredCharacterSettings << characterSetting;
    }

    progress.setValue(characterSettings.size());

    return filteredCharacterSettings;
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

QList<QList<CharacterSetting>> SettingRanking::classifyByClassEngrave(const QList<CharacterSetting> &characterSettings)
{
    QList<QList<CharacterSetting>> classifiedByClassEngrave(3);

    QProgressDialog progress("직업 각인 분류중...", QString(), 0, characterSettings.size(), nullptr);
    progress.setWindowModality(Qt::WindowModal);

    for (int i = 0; i < characterSettings.size(); i++)
    {
        progress.setValue(i);
        QCoreApplication::processEvents();

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

    progress.setValue(characterSettings.size());

    return classifiedByClassEngrave;
}

void SettingRanking::classifyBySetting(const QList<QList<CharacterSetting>> &characterSettings)
{
    QProgressDialog progress("세팅 ID 생성중", QString(), 0, characterSettings.size(), nullptr);
    progress.setWindowModality(Qt::WindowModal);

    // setting id를 부여하여 분류
    for (int i = 0; i < characterSettings.size(); i++)
    {
        progress.setValue(i);
        QCoreApplication::processEvents();

        for (const CharacterSetting &characterSetting : characterSettings[i])
        {
            QString settingId;

            settingId += characterSetting.itemSet;
            settingId += characterSetting.engrave;
            settingId += characterSetting.engraveLevel;
            settingId += characterSetting.ability;
            settingId += characterSetting.elixir;

            mCharacterSettings[i][settingId] << characterSetting;
        }
    }

    progress.setValue(characterSettings.size());

    // setting id에 해당하는 목록의 size 계산 및 내림차순 정렬
    for (int i = 0; i < mCharacterSettings.size(); i++)
    {
        for (auto iter = mCharacterSettings[i].begin(); iter != mCharacterSettings[i].end(); iter++)
        {
            const QString &settingId = iter.key();
            int size = mCharacterSettings[i][settingId].size();

            mSettingCounts[i].append({settingId, size});
        }

        std::sort(mSettingCounts[i].begin(), mSettingCounts[i].end(), [&](auto &a, auto &b)
        {
            return a.second > b.second;
        });
    }
}

void SettingRanking::showCharacterSettings()
{
    for (QWidget *pWidget : mOutputs)
        delete pWidget;
    mOutputs.clear();

    ui->labelInfo->setText(QString("검색된 캐릭터 수 : %1").arg(mTotalDataCount));

    QList<QVBoxLayout *> layouts = {ui->vLayoutOutput1, ui->vLayoutOutput2, ui->vLayoutOutput3};

    // 직업 각인별 이용률 출력
    for (int i = 0; i < mCharacterSettings.size(); i++)
    {
        QString classEngrave = i == 2 ? "쌍직각" :
                                        EngraveManager::getInstance()->getEngraveByCode(mClassEngraveCodes[i]);
        double usageRate = (mDataCounts[i] / static_cast<double>(mTotalDataCount)) * 100;

        QLabel *pLabelUsageRate = WidgetManager::createLabel(QString("%1 (%2%)").arg(classEngrave).arg(usageRate, 0, 'f', 2),
                                                             14);
        pLabelUsageRate->setStyleSheet("QLabel { border-radius: 5px;"
                                       "         padding: 2px;"
                                       "         background-color: black; "
                                       "         color: white; }");
        layouts[i]->addWidget(pLabelUsageRate);
        layouts[i]->setAlignment(pLabelUsageRate, Qt::AlignHCenter);
        mOutputs << pLabelUsageRate;
    }

    // 세팅 목록 출력
    for (int i = 0; i < mCharacterSettings.size(); i++)
    {
        const QList<QPair<QString, int>> settingCounts = mSettingCounts[i];

        QProgressDialog progress("데이터 출력중", QString(), 0, settingCounts.size(), nullptr);
        progress.setWindowModality(Qt::WindowModal);

        for (int j = 0; j < settingCounts.size(); j++)
        {
            progress.setValue(j);
            QCoreApplication::processEvents();

            const QString &settingId = settingCounts[j].first;
            int settingCount = settingCounts[j].second;

            QFrame *pHLine = WidgetManager::createLine(QFrame::HLine);
            layouts[i]->addWidget(pHLine);
            mOutputs << pHLine;

            SettingInfo *pSettingInfo = new SettingInfo(mCharacterSettings[i][settingId].first(),
                                                        j + 1, settingCount, mDataCounts[i]);
            layouts[i]->addWidget(pSettingInfo);
            layouts[i]->setAlignment(pSettingInfo, Qt::AlignHCenter);
            mOutputs << pSettingInfo;
        }

        progress.setValue(settingCounts.size());
    }

    mpSearchButton->setEnabled(true);
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
