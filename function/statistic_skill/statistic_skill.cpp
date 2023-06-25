#include "statistic_skill.h"
#include "ui_statistic_skill.h"
#include "ui/widget_manager.h"
#include "api/api_manager.h"
#include "game/engrave/engrave_manager.h"
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
    pClassSelector(nullptr)
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
}

QGroupBox *StatisticSkill::createClassSelector()
{
    QGroupBox *pGroupBox = WidgetManager::createGroupBox("직업 선택");

    QHBoxLayout *pLayout = new QHBoxLayout();
    pGroupBox->setLayout(pLayout);

    pClassSelector = WidgetManager::createComboBox({});
    connect(pClassSelector, &QComboBox::currentTextChanged,
            this, &StatisticSkill::updateEngraveButton);
    connect(pClassSelector, &QComboBox::currentTextChanged,
            this, [&](const QString &className)
    {
        mSelectedClass = className;
    });
    pLayout->addWidget(pClassSelector);

    QNetworkAccessManager *pClassLoader = new QNetworkAccessManager;
    connect(pClassLoader, &QNetworkAccessManager::finished,
            pClassLoader, &QNetworkAccessManager::deleteLater);
    connect(pClassLoader, &QNetworkAccessManager::finished,
            this, [&](QNetworkReply *pReply)
    {
        QJsonArray classes = QJsonDocument::fromJson(pReply->readAll()).array();
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

void StatisticSkill::updateEngraveButton(const QString &className)
{
    enableInput(false);

    const QStringList &classEngraves = EngraveManager::getInstance()->getClassEngraves(className);

    for (int i = 0; i < classEngraves.size(); i++)
        mClassEngraveButtons[i]->setText(classEngraves[i]);

    enableInput(true);
}

void StatisticSkill::initializeClassEngraveLayout()
{
    ui->hLayoutClassEngrave->setAlignment(Qt::AlignHCenter);

    for (int i = 0; i < 2; i++)
        ui->hLayoutClassEngrave->addWidget(createClassEngraveButton());
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

        for (QPushButton *pButton : mClassEngraveButtons)
            pButton->setStyleSheet("");

        pClassEngraveButton->setStyleSheet("QPushButton { "
                                           "border: 2px solid blue;"
                                           "border-radius: 5px; "
                                           "}");

        searchStatistic(pClassEngraveButton->text());
    });

    return pClassEngraveButton;
}

void StatisticSkill::searchStatistic(const QString &classEngrave)
{
    QNetworkAccessManager *pStatisticLoader = new QNetworkAccessManager;

    connect(pStatisticLoader, &QNetworkAccessManager::finished,
            pStatisticLoader, &QNetworkAccessManager::deleteLater);
    connect(pStatisticLoader, &QNetworkAccessManager::finished,
            this, &StatisticSkill::parseStatistic);

    ApiManager::getInstance()->getStatistics(
        pStatisticLoader, Statistics::Skill, {classEngrave});
}

void StatisticSkill::parseStatistic(QNetworkReply *pReply)
{
    if (!ApiManager::getInstance()->handleStatusCode(pReply)) {
        enableInput(true);
        return;
    }

    clearSkillUsageWidget();

    const QJsonObject response = QJsonDocument::fromJson(pReply->readAll()).object();
    const double &count = response.find("count")->toDouble();
    const QJsonArray &skillCounts = response.find("skillCounts")->toArray();

    for (auto iter = skillCounts.constBegin(); iter != skillCounts.constEnd(); iter++)
    {
        const int skillCount = iter->toObject().find("count")->toInt();
        const QString &skillName = iter->toObject().find("skillName")->toString();
        const QJsonArray &skillLevelCounts = iter->toObject().find("skillLevelCounts")->toArray();
        const QJsonArray &tripodCounts = iter->toObject().find("tripodCounts")->toArray();
        const QJsonArray &runeCounts = iter->toObject().find("runeCounts")->toArray();

        SkillUsageWidget *pSkillUsageWidget = new SkillUsageWidget(
            mSelectedClass, count, skillCount, skillName,
            parseSkillLevel(skillLevelCounts),
            parseTripod(tripodCounts),
            parseRune(runeCounts)
        );
        ui->vLayoutResult->addWidget(pSkillUsageWidget);
        mSkillUsageWidgets << pSkillUsageWidget;

        QFrame *pHLine = WidgetManager::createLine(QFrame::HLine);
        ui->vLayoutResult->addWidget(pHLine);
        mLines << pHLine;
    }

    enableInput(true);
}

QList<QPair<int, int> > StatisticSkill::parseSkillLevel(const QJsonArray &skillLevelCounts)
{
    QList<QPair<int, int>> result;

    for (auto iter = skillLevelCounts.constBegin(); iter != skillLevelCounts.constEnd(); iter++)
    {
        const QJsonObject &skillLevelCount = iter->toObject();

        result.append({
            skillLevelCount.find("count")->toInt(),
            skillLevelCount.find("skillLevel")->toInt()
        });
    }

    return result;
}

QList<QPair<int, QString> > StatisticSkill::parseTripod(const QJsonArray &tripodCounts)
{
    QList<QPair<int, QString>> result;

    for (auto iter = tripodCounts.constBegin(); iter != tripodCounts.constEnd(); iter++)
    {
        const QJsonObject &tripodCount = iter->toObject();

        result.append({
            tripodCount.find("count")->toInt(),
            tripodCount.find("tripodName")->toString()
        });
    }

    return result;
}

QList<QPair<int, QString> > StatisticSkill::parseRune(const QJsonArray &runeCounts)
{
    QList<QPair<int, QString>> result;

    for (auto iter = runeCounts.constBegin(); iter != runeCounts.constEnd(); iter++)
    {
        const QJsonObject &runeCount = iter->toObject();

        result.append({
            runeCount.find("count")->toInt(),
            runeCount.find("runeName")->toString()
        });
    }

    return result;
}

void StatisticSkill::clearSkillUsageWidget()
{
    for (SkillUsageWidget *pSkillUsageWidget : mSkillUsageWidgets)
        delete pSkillUsageWidget;

    mSkillUsageWidgets.clear();

    for (QFrame *pLine : mLines)
        delete pLine;

    mLines.clear();
}

void StatisticSkill::enableInput(bool enable)
{
    pClassSelector->setEnabled(enable);

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
