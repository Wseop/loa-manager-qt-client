#include "skill_stats.h"
#include "ui_skill_stats.h"
#include "ui/widget_manager.h"
#include "ui/font_manager.h"
#include "game/engrave/engrave_manager.h"
#include "game/skill/skill_manager.h"
#include "function/skill_stats/skill_stats_widget.h"
#include "api/api_manager.h"
#include "api/loamanager/loamanager_api.h"
#include "api/response_parser.h"

#include <QLabel>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QFrame>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProgressDialog>

#include <algorithm>

SkillStats *SkillStats::mpInstance = nullptr;

SkillStats::SkillStats() :
    ui(new Ui::SkillStats)
{
    ui->setupUi(this);

    initializeMenuLayout();
    initializeInfoLabel();
    initializeClassEngraveSelector();
}

SkillStats::~SkillStats()
{
    delete ui;
}

void SkillStats::initializeMenuLayout()
{
    ui->hLayoutMenu->setAlignment(Qt::AlignHCenter);

    initializeClassSelector();
    initializeSearchButton();
}

void SkillStats::initializeClassSelector()
{
    QGroupBox *pGroupClassSelect = WidgetManager::createGroupBox("직업 선택");
    QHBoxLayout *pGroupLayout = new QHBoxLayout();
    pGroupClassSelect->setLayout(pGroupLayout);
    ui->hLayoutMenu->addWidget(pGroupClassSelect);

    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&, pGroupLayout](QNetworkReply *pReply){
        QJsonArray classes = QJsonDocument::fromJson(pReply->readAll()).array();
        QStringList classNames;

        for (const QJsonValue &value : classes) {
            classNames << value.toObject().find("child")->toVariant().toStringList();
        }

        mpClassSelector = WidgetManager::createComboBox(classNames);
        pGroupLayout->addWidget(mpClassSelector);
    });
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    ApiManager::getInstance()->get(pNetworkManager,
                                   ApiType::LoaManager,
                                   static_cast<int>(LoamanagerApi::GetResource),
                                   {"class", ""},
                                   "");
}

void SkillStats::initializeSearchButton()
{
    mpSearchButton = WidgetManager::createPushButton("검색");
    ui->hLayoutMenu->addWidget(mpSearchButton);

    connect(mpSearchButton, &QPushButton::released, this, [&]()
    {
        const QString &className = mpClassSelector->currentText();

        mpSearchButton->setDisabled(true);

        setClassEngraveIcon(className);
        searchSkillSettings(className);
        initializeSkillLayout(className);

        ui->labelInfo->show();

        for (QPushButton *pClassEngraveSelector : mClassEngraveSelectors)
        {
            pClassEngraveSelector->hide();
            pClassEngraveSelector->setStyleSheet("QPushButton {}");
        }
    });
}

void SkillStats::initializeInfoLabel()
{
    QFont font = FontManager::getInstance()->getFont(FontFamily::NanumSquareNeoBold, 12);

    ui->labelInfo->setFont(font);
    ui->labelInfo->setStyleSheet("QLabel { color: red }");
    ui->labelInfo->setText("스킬 데이터 불러오는 중...");

    ui->labelInfo->hide();
}

void SkillStats::initializeClassEngraveSelector()
{
    ui->hLayoutClassEngrave->setAlignment(Qt::AlignHCenter);

    const QStringList buttonNames = {"직업 각인 1", "직업 각인 2", "쌍직각"};

    for (int i = 0; i < buttonNames.size(); i++)
    {
        QPushButton *pClassEngraveSelector = WidgetManager::createPushButton(buttonNames[i]);

        ui->hLayoutClassEngrave->addWidget(pClassEngraveSelector);
        mClassEngraveSelectors << pClassEngraveSelector;

        pClassEngraveSelector->hide();

        connect(pClassEngraveSelector, &QPushButton::released, this, [&, i]()
        {
            for (int j = 0; j < mClassEngraveSelectors.size(); j++)
            {
                if (j == i)
                {
                    mClassEngraveSelectors[j]->setStyleSheet("QPushButton { border: 2px solid blue;"
                                                             "              border-radius: 5px }");
                }
                else
                {
                    mClassEngraveSelectors[j]->setStyleSheet("QPushButton {}");
                }
            }

            // 선택한 직업 각인으로 스킬 usage 업데이트
            for (auto &skillStatsWidget : mSkillStatsWidgets)
            {
                if (mSkillUsageInfos[i].contains(skillStatsWidget.first)) {
                    skillStatsWidget.second->updateUsage(mSettingCounts[i], mSkillUsageInfos[i][skillStatsWidget.first]);
                }
            }

            // 사용률이 높은 순으로 정렬
            std::sort(mSkillStatsWidgets.begin(), mSkillStatsWidgets.end(), [&](auto &a, auto &b)
            {
                return a.second->getSkillUsage() > b.second->getSkillUsage();
            });

            // 정렬한 결과로 재출력
            for (auto &skillStatsWidget : mSkillStatsWidgets)
            {
                ui->vLayoutSkill->removeWidget(skillStatsWidget.second);
            }

            for (QFrame *pLine : mLines)
            {
                delete pLine;
            }
            mLines.clear();

            for (auto &skillStatsWidget : mSkillStatsWidgets)
            {
                ui->vLayoutSkill->addWidget(skillStatsWidget.second);

                QFrame *pLine = WidgetManager::createLine(QFrame::HLine);
                ui->vLayoutSkill->addWidget(pLine);
                mLines << pLine;
            }
        });
    }
}

void SkillStats::setClassEngraveIcon(const QString &className)
{
    static EngraveManager *pEngraveManager = EngraveManager::getInstance();

    const QStringList classEngraves = pEngraveManager->getClassEngraves(className);

    // 직업각인 선택 버튼에 아이콘 세팅
    for (int i = 0; i < classEngraves.size(); i++)
    {
        QPushButton *pClassEngraveSelector = mClassEngraveSelectors[i];

        pClassEngraveSelector->setIcon(QPixmap(pEngraveManager->iconPath(classEngraves[i])));
        pClassEngraveSelector->setIconSize(QSize(35, 35));
        pClassEngraveSelector->setText("");

        mClassEngraveIndex[classEngraves[i]] = i;
    }
}

void SkillStats::initializeSkillLayout(const QString &className)
{
    // 데이터 초기화
    for (auto &skillStatsWidget : mSkillStatsWidgets)
    {
        delete skillStatsWidget.second;
    }
    mSkillStatsWidgets.clear();

    for (QFrame *pLine : mLines)
    {
        delete pLine;
    }
    mLines.clear();

    static SkillManager *pSkillManager = SkillManager::getInstance();

    const QHash<QString, Skill> skills = pSkillManager->skills(className);

    // 스킬 정보 widget 추가
    for (auto iter = skills.begin(); iter != skills.end(); iter++)
    {
        SkillStatsWidget *pSkillStatsWidget = new SkillStatsWidget(iter.value());
        ui->vLayoutSkill->addWidget(pSkillStatsWidget);
        mSkillStatsWidgets.append({iter.key(), pSkillStatsWidget});

        QFrame *pLine = WidgetManager::createLine(QFrame::HLine);
        ui->vLayoutSkill->addWidget(pLine);
        mLines << pLine;
    }
}

void SkillStats::searchSkillSettings(const QString &className)
{
    // 데이터 초기화
    mSettingCounts = QList<int>(3, 0);
    mSkillUsageInfos = QList<QHash<QString, SkillUsageInfo>>(3);

    // 스킬 세팅 정보 로드
    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished, this, &SkillStats::updateSkillSettings);
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    ApiManager::getInstance()->get(pNetworkManager,
                                   ApiType::LoaManager,
                                   static_cast<int>(LoamanagerApi::GetStats),
                                   {"skill", className},
                                   "");
}

void SkillStats::updateSkillSettings(QNetworkReply *pReply)
{
    QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());

    if (response.isNull())
        return;

    QList<SkillSetting> skillSettings = ResponseParser::parseSkillSettings(response);

    QList<QHash<QString, QHash<QString, int>>> tripodUsage(3);
    QList<QHash<QString, QHash<QString, int>>> runeUsage(3);

    for (int i = 0; i < skillSettings.size(); i++)
    {
        if (skillSettings[i].classEngraves.size() == 0)
            continue;

        int index = skillSettings[i].classEngraves.size() == 2
                    ? 2
                    : mClassEngraveIndex[skillSettings[i].classEngraves[0]];

        const QList<SkillSetting::SkillData> &skillDatas = skillSettings[i].skills;

        // 스킬 usage 정보 업데이트
        for (const auto &skillData : skillDatas)
        {
            // 스킬 usage
            mSkillUsageInfos[index][skillData.skillName].skillUsage++;

            // 트라이포드 usage
            for (const QString &tripodName : skillData.tripodsNames)
            {
                tripodUsage[index][skillData.skillName][tripodName]++;
            }

            // 룬 usage
            if (skillData.runeName != "")
            {
                runeUsage[index][skillData.skillName][skillData.runeName]++;
            }
        }

        mSettingCounts[index]++;
    }

    for (int i = 0; i < mSettingCounts.size(); i++)
    {
        for (auto iter = tripodUsage[i].begin(); iter != tripodUsage[i].end(); iter++)
        {
            const QString &skillName = iter.key();
            const QHash<QString, int> &tripods = iter.value();

            for (auto iterTripod = tripods.begin(); iterTripod != tripods.end(); iterTripod++)
            {
                mSkillUsageInfos[i][skillName].tripodUsage.append({iterTripod.key(), iterTripod.value()});
            }
        }

        for (auto iter = runeUsage[i].begin(); iter != runeUsage[i].end(); iter++)
        {
            const QString &skillName = iter.key();
            const QHash<QString, int> &runes = iter.value();

            for (auto iterRune = runes.begin(); iterRune != runes.end(); iterRune++)
            {
                mSkillUsageInfos[i][skillName].runeUsage.append({iterRune.key(), iterRune.value()});

                std::sort(mSkillUsageInfos[i][skillName].runeUsage.begin(),
                          mSkillUsageInfos[i][skillName].runeUsage.end(),
                          [&](auto &a, auto &b)
                {
                    return a.second > b.second;
                });
            }
        }
    }

    for (int i = 0; i < mSettingCounts.size(); i++)
    {
        if (mSettingCounts[i] > 0)
        {
            mClassEngraveSelectors[i]->show();
        }
    }

    ui->labelInfo->hide();

    mpSearchButton->setEnabled(true);
}

void SkillStats::start()
{

}

SkillStats *SkillStats::getInstance()
{
    if (mpInstance == nullptr)
        mpInstance = new SkillStats();

    return mpInstance;
}

void SkillStats::destroyInstance()
{
    if (mpInstance == nullptr)
        return;

    delete mpInstance;
    mpInstance = nullptr;
}
