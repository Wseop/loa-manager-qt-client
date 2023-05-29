#include "setting_ranking.h"
#include "ui_setting_ranking.h"
#include "ui/widget_manager.h"
#include "ui/font_manager.h"
#include "api/api_manager.h"
#include "game/engrave/engrave_manager.h"
#include "function/setting_ranking/setting_info.h"

#include <QLabel>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
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
    mpSearchButton(nullptr),
    mOutputs(static_cast<int>(SettingCategory::size))
{
    ui->setupUi(this);

    initializeInputLayout();
    initializeOutputLayout();
    initializeCategoryButton();
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
    QGroupBox *pGroupBox = WidgetManager::createGroupBox("직업 선택");
    ui->hLayoutInput->addWidget(pGroupBox);
    ui->hLayoutInput->setAlignment(pGroupBox, Qt::AlignCenter);

    QHBoxLayout *pHLayout = new QHBoxLayout();
    pGroupBox->setLayout(pHLayout);

    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&, pHLayout](QNetworkReply *pReply){
        QJsonArray classes = QJsonDocument::fromJson(pReply->readAll()).array();
        QStringList classNames;

        for (const QJsonValue &value : classes) {
            classNames << value.toObject().find("child")->toVariant().toStringList();
        }

        mpClassSelector = WidgetManager::createComboBox(classNames);
        pHLayout->addWidget(mpClassSelector);
    });
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

//    ApiManager::getInstance()->get(pNetworkManager,
//                                   ApiType::LoaManager,
//                                   static_cast<int>(LoamanagerApi::GetResource),
//                                   {"class", ""},
//                                   "");
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
        for (int i = 0; i < mOutputs.size(); i++)
        {
            for (int j = 0; j < mOutputs[i].size(); j++)
            {
                delete mOutputs[i][j];
            }
        }
        mOutputs = QList<QList<QWidget*>>(static_cast<int>(SettingCategory::size));

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
        });
        connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

//        ApiManager::getInstance()->get(pNetworkManager,
//                                       ApiType::LoaManager,
//                                       static_cast<int>(LoamanagerApi::GetStats),
//                                       {"setting", mpClassSelector->currentText()},
//                                       "");
    });

    ui->hLayoutInput->addWidget(mpSearchButton);
    ui->hLayoutInput->setAlignment(mpSearchButton, Qt::AlignCenter);
}

void SettingRanking::initializeHelp()
{
    QString help = "[검색 조건]\n1620이상, 엘릭서 세트효과 활성화\n배신세트 제외\n특성 제인숙 제외";
    QLabel *pLabelHelp = WidgetManager::createLabel(help, 10, 300, 75);

    ui->hLayoutInput->addWidget(pLabelHelp);
}

void SettingRanking::initializeOutputLayout()
{
    mLines = {ui->line_2, ui->line_3, ui->line_4, ui->line_5, ui->line_6, ui->line_7};

    for (int i = 0; i < mLines.size(); i++)
    {
        mLines[i]->hide();
    }

    mClassEngraveLayouts = {ui->vLayoutClassEngrave1, ui->vLayoutClassEngrave2, ui->vLayoutClassEngrave3};
    mItemSetLayouts = {ui->vLayoutItemSet1, ui->vLayoutItemSet2, ui->vLayoutItemSet3};
    mAbilityLayouts = {ui->vLayoutAbility1, ui->vLayoutAbility2, ui->vLayoutAbility3};
    mElixirLayouts = {ui->vLayoutElixir1, ui->vLayoutElixir2, ui->vLayoutElixir3};
    mEngraveSingleLayouts = {ui->vLayoutEngraveSingle1, ui->vLayoutEngraveSingle2, ui->vLayoutEngraveSingle3};
    mEngraveLayouts = {ui->vLayoutEngrave1, ui->vLayoutEngrave2, ui->vLayoutEngrave3};
    mOverallLayouts = {ui->vLayoutOverall1, ui->vLayoutOverall2, ui->vLayoutOverall3};

    for (int i = 0; i < 3; i++)
    {
        mClassEngraveLayouts[i]->setAlignment(Qt::AlignTop);
        mItemSetLayouts[i]->setAlignment(Qt::AlignTop);
        mAbilityLayouts[i]->setAlignment(Qt::AlignTop);
        mElixirLayouts[i]->setAlignment(Qt::AlignTop);
        mEngraveSingleLayouts[i]->setAlignment(Qt::AlignTop);
        mEngraveLayouts[i]->setAlignment(Qt::AlignTop);
        mOverallLayouts[i]->setAlignment(Qt::AlignTop);
    }

    ui->labelInfo->setFont(FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, 16));
}

void SettingRanking::initializeCategoryButton()
{
    const QList<QPushButton*> buttons = {nullptr, ui->pbItemSet, ui->pbAbility, ui->pbElixir, ui->pbEngraveSingle, ui->pbEngrave, ui->pbOverall};
    const QFont font = FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, 18);

    for (int i = 0; i < buttons.size(); i++)
    {
        QPushButton *pButton = buttons[i];

        if (pButton == nullptr)
            continue;

        pButton->setFont(font);

        connect(pButton, &QPushButton::released, this, [&, i]()
        {
            QList<QWidget*> &widgets = mOutputs[i];

            for (QWidget *pWidget : widgets)
            {
                if (pWidget->isHidden())
                    pWidget->setVisible(true);
                else
                    pWidget->setHidden(true);
            }
        });
    }
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

void SettingRanking::showClassEngraveRatio()
{
    static const QString labelStyle = "QLabel { border-radius: 5px;"
                                      "         padding: 2px;"
                                      "         background-color: black; "
                                      "         color: #E4BA27; }";

    for (int i = 0; i < 3; i++)
    {
        QString classEngrave = i == 2 ? "쌍직각" :
                                        EngraveManager::getInstance()->getEngraveName(mClassEngraveCodes[i]);
        double usageRate = (mDataCounts[i] / static_cast<double>(mTotalDataCount)) * 100;

        if (usageRate == 0)
            continue;

        QLabel *pLabelUsageRate = WidgetManager::createLabel(QString("%1 (%2%)").arg(classEngrave).arg(usageRate, 0, 'f', 2),
                                                             16, 500, 50);
        pLabelUsageRate->setStyleSheet(labelStyle);
        mClassEngraveLayouts[i]->addWidget(pLabelUsageRate);
        mOutputs[static_cast<int>(SettingCategory::ClassEngrave)] << pLabelUsageRate;
    }
}

void SettingRanking::showRatio(QString textColor, int index, SettingCategory category, const QList<QVBoxLayout *> &layouts, const QList<QPair<QString, int> > &classifiedData)
{
    const QString labelStyle = QString("QLabel { border-radius: 5px;"
                                              "         padding: 2px;"
                                              "         background-color: black; "
                                              "         color: %1; }").arg(textColor);

    for (int i = 0; i < classifiedData.size(); i++)
    {
        double usageRate = (classifiedData[i].second / static_cast<double>(mDataCounts[index])) * 100;

        QLabel *pLabelUsageRate = WidgetManager::createLabel(QString("%1 (%2%)")
                                                           .arg(classifiedData[i].first)
                                                           .arg(usageRate, 0, 'f', 2),
                                                           14, 500);
        pLabelUsageRate->setStyleSheet(labelStyle);
        layouts[index]->addWidget(pLabelUsageRate);
        mOutputs[static_cast<int>(category)] << pLabelUsageRate;
    }
}

void SettingRanking::showEngraveRatio(int index, const QList<QPair<QString, int> > &engraveRatio)
{
    static const QString labelStyle = "QLabel { border-radius: 5px;"
                                      "         padding: 2px;"
                                      "         background-color: black; "
                                      "         color: #FFFFAC; }";

    static EngraveManager *pEngraveManager = EngraveManager::getInstance();

    for (int i = 0; i < engraveRatio.size(); i++)
    {
        if (i == 10)
            break;

        QLabel *pLabelUsageRate = WidgetManager::createLabel("", 14, 500);
        pLabelUsageRate->setStyleSheet(labelStyle);
        mEngraveLayouts[index]->addWidget(pLabelUsageRate);
        mOutputs[static_cast<int>(SettingCategory::Engrave)] << pLabelUsageRate;

        // 각인 및 레벨 출력
        QStringList engraveSetting = engraveRatio[i].first.split(',');
        QStringList engraveSummaries(3, "");

        for (int level = 3; level > 0; level--)
        {
            for (int j = 0; j < engraveSetting[1].size(); j++)
            {
                const QString &engrave = pEngraveManager->getEngraveName(engraveSetting[0].sliced(j * 3, 3).toInt());
                int engraveLevel = engraveSetting[1][j].digitValue();

                if (engraveLevel != level)
                    continue;

                QLabel *pLabelEngrave = WidgetManager::createLabel(QString("%1 Lv.%2").arg(engrave).arg(engraveLevel), 10, 500);
                mEngraveLayouts[index]->addWidget(pLabelEngrave);
                mOutputs[static_cast<int>(SettingCategory::Engrave)] << pLabelEngrave;

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
    for (int i = 0; i < overallRatio.size(); i++)
    {
        QFrame *pHLine = WidgetManager::createLine(QFrame::HLine);
        mOverallLayouts[index]->addWidget(pHLine);
        mOutputs[static_cast<int>(SettingCategory::Overall)] << pHLine;

        const QStringList settings = overallRatio[i].first.split(",");

//        CharacterSetting characterSetting;
//        characterSetting.itemSet = settings[0];
//        characterSetting.ability = settings[1];
//        characterSetting.elixir = settings[2];
//        characterSetting.engrave = settings[3];
//        characterSetting.engraveLevel = settings[4];

        //SettingInfo *pSettingInfo = new SettingInfo(characterSetting, i + 1, overallRatio[i].second, mDataCounts[index]);
        //mOverallLayouts[index]->addWidget(pSettingInfo);
        //mOutputs[static_cast<int>(SettingCategory::Overall)] << pSettingInfo;
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
