#include "statistic_skill.h"
#include "ui_statistic_skill.h"
#include "ui/widget_manager.h"
#include "api/api_manager.h"
#include "game/engrave/engrave_manager.h"
#include "game/skill/skill_manager.h"
#include "function/statistic_skill/skill_usage_widget.h"

#include <QGroupBox>
#include <QPushButton>
#include <QComboBox>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <algorithm>

StatisticSkill *StatisticSkill::pInstance = nullptr;

StatisticSkill::StatisticSkill() :
    ui(new Ui::StatisticSkill),
    pClassSelector(nullptr),
    pSearchButton(nullptr)
{
    ui->setupUi(this);
    ui->vLayoutMain->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    initializeClassLayout();
    initializeClassEngraveLayout();
}

StatisticSkill::~StatisticSkill()
{
    delete ui;
}

void StatisticSkill::initializeClassLayout()
{
    ui->hLayoutClass->setAlignment(Qt::AlignHCenter);
    ui->hLayoutClass->addWidget(createClassSelector());
    ui->hLayoutClass->addWidget(createSearchButton());
}

QGroupBox *StatisticSkill::createClassSelector()
{
    QGroupBox *pGroupBox = WidgetManager::createGroupBox("직업 선택");

    QHBoxLayout *pLayout = new QHBoxLayout();
    pGroupBox->setLayout(pLayout);

    pClassSelector = WidgetManager::createComboBox({});
    connect(pClassSelector, &QComboBox::currentTextChanged,
            this, [&](const QString &text)
            {
                mSelectedClassName = text;
            });
    pLayout->addWidget(pClassSelector);

    QNetworkAccessManager *pClassLoader = new QNetworkAccessManager;
    connect(pClassLoader, &QNetworkAccessManager::finished,
            pClassLoader, &QNetworkAccessManager::deleteLater);
    connect(pClassLoader, &QNetworkAccessManager::finished,
            this, [&](QNetworkReply *pReply)
            {
                QJsonArray classes = QJsonDocument::fromJson(
                                 pReply->readAll()).array();
                QStringList classNames;

                for (const QJsonValue &value : classes) {
                    classNames << value.toObject().find("child")->toVariant()
                                      .toStringList();
                }

                pClassSelector->addItems(classNames);
            });

    ApiManager::getInstance()->getResources(pClassLoader, Resources::Class, "");

    return pGroupBox;
}

QPushButton *StatisticSkill::createSearchButton()
{
    pSearchButton = WidgetManager::createPushButton("검색");

    connect(pSearchButton, &QPushButton::released,
            this, &StatisticSkill::searchStatistic);
    connect(pSearchButton, &QPushButton::released,
            this, &StatisticSkill::clearSkillUsageWidget);

    return pSearchButton;
}

void StatisticSkill::initializeClassEngraveLayout()
{
    ui->hLayoutClassEngrave->setAlignment(Qt::AlignHCenter);

    for (int i = 0; i < 3; i++) {
        ui->hLayoutClassEngrave->addWidget(createClassEngraveButton());
    }
}

QPushButton *StatisticSkill::createClassEngraveButton()
{
    QPushButton *pClassEngraveButton = WidgetManager::createPushButton("-", 10, 125);
    mClassEngraveButtons << pClassEngraveButton;

    connect(pClassEngraveButton, &QPushButton::released, this, [&, pClassEngraveButton]()
    {
        enableInput(false);

        if (pClassEngraveButton->text() == "-") {
            enableInput(true);
            return;
        }

        for (QPushButton *pButton : mClassEngraveButtons) {
            pButton->setStyleSheet("");
        }

        pClassEngraveButton->setStyleSheet("QPushButton { border: 2px solid blue;"
                                           "              border-radius: 5px; }");

        setSkillUsageWidget(pClassEngraveButton->text());
        enableInput(true);
    });

    return pClassEngraveButton;
}

void StatisticSkill::setClassEngraveButtonText(int index, const QString &classEngrave)
{
    mClassEngraveButtons[index]->setText(classEngrave);
    mClassEngraveButtons[index]->setStyleSheet("");
}

void StatisticSkill::searchStatistic()
{
    enableInput(false);

    QNetworkAccessManager *pStatisticLoader = new QNetworkAccessManager;

    connect(pStatisticLoader, &QNetworkAccessManager::finished,
            pStatisticLoader, &QNetworkAccessManager::deleteLater);
    connect(pStatisticLoader, &QNetworkAccessManager::finished,
            this, &StatisticSkill::parseStatistic);

    ApiManager::getInstance()->getStatistics(
        pStatisticLoader, Statistics::SettingsSkill, mSelectedClassName);
}

void StatisticSkill::parseStatistic(QNetworkReply *pReply)
{
    if (!ApiManager::getInstance()->handleStatusCode(pReply)) {
        enableInput(true);
        return;
    }

    QStringList classEngraves = EngraveManager::getInstance()
                                    ->getClassEngraves(mSelectedClassName);
    classEngraves << "쌍직각";

    SettingsSkill statisticData;
    QJsonObject response = QJsonDocument::fromJson(pReply->readAll()).object();

    statisticData.count = response.find("count")->toInt();

    for (int i = 0; i < classEngraves.size(); i++) {
        const QJsonObject &skills = response.find(classEngraves[i])->toObject();

        statisticData.settings[classEngraves[i]].count = skills.find("count")->toInt();

        for (auto iter = skills.constBegin(); iter != skills.constEnd(); iter++) {
            if (iter.key() == "count")
                continue;

            const QJsonObject &skill = iter.value().toObject();
            const QJsonObject &levels = skill.find("levels")->toObject();
            const QJsonObject &tripods = skill.find("tripods")->toObject();
            const QJsonObject &runes = skill.find("runes")->toObject();

            SkillUsage skillUsage;
            skillUsage.count = skill.find("count")->toInt();

            for (auto levelIter = levels.constBegin(); levelIter != levels.constEnd(); levelIter++) {
                skillUsage.levels[levelIter.key().toInt()] = levelIter.value().toInt();
            }

            for (auto tripodIter = tripods.constBegin(); tripodIter != tripods.constEnd(); tripodIter++) {
                skillUsage.tripods[tripodIter.key()] = tripodIter.value().toInt();
            }

            for (auto runeIter = runes.constBegin(); runeIter != runes.constEnd(); runeIter++) {
                if (runeIter.key() == "null") {
                    skillUsage.runes.append({"미착용", runeIter.value().toInt()});
                } else {
                    skillUsage.runes.append({runeIter.key(), runeIter.value().toInt()});
                }
            }

            // 룬 사용률 내림차순 정렬
            std::sort(skillUsage.runes.begin(), skillUsage.runes.end(), [&](auto a, auto b){
                return a.second > b.second;
            });

            statisticData.settings[classEngraves[i]].skillUsages.append({
                iter.key(), skillUsage
            });
        }

        setClassEngraveButtonText(i, classEngraves[i]);
    }

    mStatisticData = statisticData;

    enableInput(true);
}

void StatisticSkill::setSkillUsageWidget(const QString &classEngraveName)
{
    clearSkillUsageWidget();

    const QHash<QString, Skill> skills = SkillManager::getInstance()->skills(
        mSelectedClassName);

    for (auto iter = skills.constBegin(); iter != skills.constEnd(); iter++) {
        SkillUsageWidget *pSkillUsageWidget = new SkillUsageWidget(iter.value());

        mSkillUsageWidgetMap[iter.value().skillName] = pSkillUsageWidget;
        mSkillUsageWidgets << pSkillUsageWidget;
    }

    updateSkillUsageRatio(classEngraveName);
}

void StatisticSkill::clearSkillUsageWidget()
{
    mSkillUsageWidgetMap.clear();

    for (SkillUsageWidget *pSkillUsageWidget : mSkillUsageWidgets) {
        delete pSkillUsageWidget;
    }

    mSkillUsageWidgets.clear();

    for (QFrame *pLine : mLines) {
        delete pLine;
    }

    mLines.clear();
}

void StatisticSkill::updateSkillUsageRatio(const QString &classEngraveName)
{
    const SettingSkill &settings = mStatisticData.settings[classEngraveName];
    const QList<QPair<QString, SkillUsage>> &skillUsages = settings.skillUsages;

    for (auto &skillUsage : skillUsages) {
        mSkillUsageWidgetMap[skillUsage.first]->updateUsageRatio(
            settings.count, skillUsage.second);
    }

    // 스킬 사용률 내림차순 정렬
    std::sort(mSkillUsageWidgets.begin(), mSkillUsageWidgets.end(),
              [&](auto a, auto b)
              {
                return a->getSkillUsageCount() > b->getSkillUsageCount();
              });

    showSkillUsageWidget();
}

void StatisticSkill::showSkillUsageWidget()
{
    for (SkillUsageWidget *pWidget : mSkillUsageWidgets) {
        ui->vLayoutResult->addWidget(pWidget);

        QFrame *hLine = WidgetManager::createLine(QFrame::HLine);
        mLines << hLine;
        ui->vLayoutResult->addWidget(hLine);
    }
}

void StatisticSkill::enableInput(bool enable)
{
    pClassSelector->setEnabled(enable);
    pSearchButton->setEnabled(enable);

    for (QPushButton *pButton : mClassEngraveButtons) {
        pButton->setEnabled(enable);
    }
}


void StatisticSkill::refresh()
{

}

StatisticSkill *StatisticSkill::getInstance()
{
    if (pInstance == nullptr)
        pInstance = new StatisticSkill();

    return pInstance;
}

void StatisticSkill::destroyInstance()
{
    if (pInstance == nullptr)
        return;

    delete pInstance;
    pInstance = nullptr;
}
