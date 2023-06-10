#include "statistic_armory.h"
#include "ui_statistic_armory.h"
#include "ui/widget_manager.h"
#include "ui/font_manager.h"
#include "api/api_manager.h"
#include "api/statistics/settings_armory.h"
#include "game/engrave/engrave_manager.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

#include <algorithm>

StatisticArmory *StatisticArmory::pInstance = nullptr;

StatisticArmory::StatisticArmory() :
    ui(new Ui::StatisticArmory),
    pClassSelector(nullptr),
    pSearchButton(nullptr)
{
    ui->setupUi(this);
    ui->vLayoutMain->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    initializeClassLayout();
    initializeClassEngraveLayout();
    initializeResultLayout();
}

StatisticArmory::~StatisticArmory()
{
    delete ui;
}

void StatisticArmory::initializeClassLayout()
{
    ui->hLayoutClass->setAlignment(Qt::AlignHCenter);
    ui->hLayoutClass->addWidget(createClassSelector());
    ui->hLayoutClass->addWidget(createSearchButton());
    ui->hLayoutClass->addWidget(createInfoLabel());
}

QGroupBox *StatisticArmory::createClassSelector()
{
    // ui 초기화
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

    // 직업 목록 load
    QNetworkAccessManager *pClassLoader = new QNetworkAccessManager;
    connect(pClassLoader, &QNetworkAccessManager::finished,
            pClassLoader, &QNetworkAccessManager::deleteLater);
    connect(pClassLoader, &QNetworkAccessManager::finished,
            this, [&](QNetworkReply *pReply)
    {
        QJsonArray classes = QJsonDocument::fromJson(pReply->readAll()).array();
        QStringList classNames;

        for (const QJsonValue &value : classes) {
            classNames << value.toObject().find("child")->toVariant().toStringList();
        }

        pClassSelector->addItems(classNames);
    });

    ApiManager::getInstance()->getResources(pClassLoader, Resources::Class, "");

    return pGroupBox;
}

QPushButton *StatisticArmory::createSearchButton()
{
    pSearchButton = WidgetManager::createPushButton("검색");

    connect(pSearchButton, &QPushButton::released,
            this, &StatisticArmory::searchStatistic);

    return pSearchButton;
}

QLabel *StatisticArmory::createInfoLabel()
{
    return WidgetManager::createLabel("데이터 수가 10개 미만일 경우\n결과에서 제외됩니다.", 10, 200, 50);
}

void StatisticArmory::initializeClassEngraveLayout()
{
    for (int i = 0; i < 2; i++) {
        ui->hLayoutEngrave->addWidget(createClassEngraveLabel());
    }
}

QLabel *StatisticArmory::createClassEngraveLabel()
{
    QLabel *pLabel = WidgetManager::createLabel("", 14, 200, 50);
    pLabel->setMaximumWidth(QWIDGETSIZE_MAX);
    mClassEngraveLabels << pLabel;
    return pLabel;
}

void StatisticArmory::initializeResultLayout()
{
    ui->vLayoutResult->setAlignment(Qt::AlignTop);

    const QStringList categories = {
        "특성", "각인(Lv.3)", "각인(Lv.2)", "각인(Lv.1)", "세트효과", "엘릭서"};

    for (const QString &category : categories) {
        ui->vLayoutResult->addWidget(createCategoryLabel(category));
        ui->vLayoutResult->addLayout(createCategoryLayout(category));
    }
}

QLabel *StatisticArmory::createCategoryLabel(const QString &category)
{
    QLabel *pLabel = WidgetManager::createLabel(category);
    pLabel->setMaximumWidth(QWIDGETSIZE_MAX);

    pLabel->setAlignment(Qt::AlignCenter);
    pLabel->setFont(FontManager::getInstance()->getFont(
        FontFamily::NanumSquareNeoBold, 16));
    pLabel->setMaximumHeight(30);
    pLabel->setStyleSheet("QLabel {"
                          "background-color: black;"
                          "color: #FFA500"
                          "}");

    return pLabel;
}

QHBoxLayout *StatisticArmory::createCategoryLayout(const QString &category)
{
    QHBoxLayout *pHLayout = new QHBoxLayout();
    QList<QVBoxLayout*> categoryLayouts;

    for (int i = 0; i < 2; i++) {
        QVBoxLayout *pVLayout = new QVBoxLayout();
        pVLayout->setAlignment(Qt::AlignTop);
        categoryLayouts << pVLayout;
        pHLayout->addLayout(pVLayout);
    }

    mCategoryLayouts[category] = categoryLayouts;

    return pHLayout;
}

void StatisticArmory::searchStatistic()
{
    enableInput(false);
    clearResult();

    QList<Statistics> categories = {
        Statistics::Ability,
        Statistics::Engrave,
        Statistics::Set,
        Statistics::Elixir
    };

    for (int i = 0; i < categories.size(); i++)
    {
        QNetworkAccessManager *pStatisticLoader = new QNetworkAccessManager;

        connect(pStatisticLoader, &QNetworkAccessManager::finished,
                pStatisticLoader, &QNetworkAccessManager::deleteLater);
        connect(pStatisticLoader, &QNetworkAccessManager::finished,
                this, [&, categories, i](QNetworkReply *pReply)
        {
            if (!ApiManager::getInstance()->handleStatusCode(pReply)) {
                enableInput(true);
                return;
            }

            if (categories[i] == Statistics::Engrave)
                parseEngraveStatistic(pReply);
            else
                parseStatistic(categories[i], pReply);
        });

        ApiManager::getInstance()->getStatistics(
            pStatisticLoader, categories[i], mSelectedClassName);
    }
}

void StatisticArmory::parseStatistic(Statistics category, QNetworkReply *pReply)
{
    QStringList classEngraves = EngraveManager::getInstance()
                                           ->getClassEngraves(mSelectedClassName);
    SettingsArmory settingsArmory;
    QJsonObject response = QJsonDocument::fromJson(pReply->readAll()).object();

    settingsArmory.count = response.find("count")->toInt();

    for (int i = 0; i < classEngraves.size(); i++)
    {
        const QJsonObject &settingCount = response.find(classEngraves[i])->toObject();

        for (auto iter = settingCount.constBegin(); iter != settingCount.constEnd(); iter++)
        {
            if (iter.key() == "count")
            {
                settingsArmory.settingCountMap[classEngraves[i]].count = iter.value()
                                                                             .toInt();
            }
            else if (iter.value().toInt() >= 10 && iter.key() != "")
            {
                settingsArmory.settingCountMap[classEngraves[i]].settingCounts.append(
                    {iter.key(), iter.value().toInt()});
            }
        }

        std::sort(settingsArmory.settingCountMap[classEngraves[i]].settingCounts.begin(),
                  settingsArmory.settingCountMap[classEngraves[i]].settingCounts.end(),
                  [&](auto a, auto b)
                  {
                      return a.second > b.second;
                  });
    }

    setSettingCount(classEngraves, category, settingsArmory);

    enableInput(true);
}

void StatisticArmory::parseEngraveStatistic(QNetworkReply *pReply)
{
    QStringList classEngraves = EngraveManager::getInstance()
                                    ->getClassEngraves(mSelectedClassName);
    QJsonArray responseArray = QJsonDocument::fromJson(pReply->readAll()).array();

    int totalCount;
    QList<int> classEngraveCounts;

    for (int level = 0; level < 3; level++)
    {
        SettingsArmory settingsArmory;
        const QJsonObject &response = responseArray[level].toObject();

        settingsArmory.count = response.find("count")->toInt();

        for (int i = 0; i < classEngraves.size(); i++)
        {
            const QJsonObject &settingCount = response.find(classEngraves[i])
                                                  ->toObject();

            for (auto iter = settingCount.constBegin(); iter != settingCount.constEnd(); iter++)
            {
                if (iter.key() == "count")
                {
                    settingsArmory.settingCountMap[classEngraves[i]].count = iter.value()
                                                                                 .toInt();
                }
                else if (iter.value().toInt() >= 10 && iter.key() != "")
                {
                    settingsArmory.settingCountMap[classEngraves[i]].settingCounts.append(
                        {iter.key(), iter.value().toInt()});
                }
            }

            std::sort(settingsArmory.settingCountMap[classEngraves[i]].settingCounts.begin(),
                      settingsArmory.settingCountMap[classEngraves[i]].settingCounts.end(),
                      [&](auto a, auto b)
                      {
                          return a.second > b.second;
                      });
        }

        setEngraveSettingCount(classEngraves, level, settingsArmory);

        if (level == 0)
        {
            totalCount = settingsArmory.count;
            classEngraveCounts
                << settingsArmory.settingCountMap[classEngraves[0]].count
                << settingsArmory.settingCountMap[classEngraves[1]].count;

            setClassEngraveCount(classEngraves, classEngraveCounts, totalCount);
        }
    }

    enableInput(true);
}

void StatisticArmory::clearResult()
{
    for (QLabel *pLabel : mResultLabels) {
        delete pLabel;
    }

    mResultLabels.clear();
}

void StatisticArmory::setSettingCount(const QStringList &classEngraves, Statistics category, SettingsArmory settingsArmory)
{
    const QString text = "%1 (%2%)";

    QString key;

    if (category == Statistics::Ability)
        key = "특성";
    else if (category == Statistics::Set)
        key = "세트효과";
    else if (category == Statistics::Elixir)
        key = "엘릭서";
    else
        return;

    for (int i = 0; i < classEngraves.size(); i++) {
        const auto &settingCounts = settingsArmory
                                        .settingCountMap[classEngraves[i]]
                                        .settingCounts;

        for (const auto &settingCount : settingCounts) {
            double ratio = settingCount.second / static_cast<double>(
                               settingsArmory.settingCountMap[classEngraves[i]].count)
                           * 100;

            QLabel *pLabel = WidgetManager::createLabel(
                text.arg(settingCount.first).arg(ratio, 0, 'f', 2), 14);
            pLabel->setMaximumWidth(QWIDGETSIZE_MAX);

            if (ratio >= 20) {
                pLabel->setStyleSheet("QLabel { color: blue }");
            }

            mCategoryLayouts[key][i]->addWidget(pLabel);
            mResultLabels << pLabel;
        }

        // 데이터가 없을 경우 layout 맞춰주기 위해 dummy label 추가
        if (settingCounts.size() == 0) {
            QLabel *pLabel = WidgetManager::createLabel("-");
            pLabel->setMaximumWidth(QWIDGETSIZE_MAX);

            mCategoryLayouts[key][i]->addWidget(pLabel);
            mResultLabels << pLabel;
        }
    }
}

void StatisticArmory::setEngraveSettingCount(const QStringList &classEngraves, int engraveLevel, SettingsArmory settingsArmory)
{
    const QString text = "%1 (%2%)";

    QString key = QString("각인(Lv.%1)").arg(engraveLevel + 1);

    for (int i = 0; i < classEngraves.size(); i++) {
        const auto &settingCounts = settingsArmory
                                        .settingCountMap[classEngraves[i]]
                                        .settingCounts;

        for (const auto &settingCount : settingCounts) {
            double ratio = settingCount.second / static_cast<double>(
                               settingsArmory.settingCountMap[classEngraves[i]].count)
                           * 100;

            QLabel *pLabel = WidgetManager::createLabel(
                text.arg(settingCount.first).arg(ratio, 0, 'f', 2), 14);
            pLabel->setMaximumWidth(QWIDGETSIZE_MAX);

            if (ratio >= 20) {
                pLabel->setStyleSheet("QLabel { color: blue }");
            }

            mCategoryLayouts[key][i]->addWidget(pLabel);
            mResultLabels << pLabel;
        }

        // 데이터가 없을 경우 layout 맞춰주기 위해 dummy label 추가
        if (settingCounts.size() == 0) {
            QLabel *pLabel = WidgetManager::createLabel("-");
            pLabel->setMaximumWidth(QWIDGETSIZE_MAX);

            mCategoryLayouts[key][i]->addWidget(pLabel);
            mResultLabels << pLabel;
        }
    }
}

void StatisticArmory::setClassEngraveCount(const QStringList &classEngraves, const QList<int> &counts, int totalCount)
{
    const QString statisticEngraveText = "%1\n(%2%)";

    for (int i = 0; i < classEngraves.size(); i++) {
        double ratio = counts[i] / static_cast<double>(totalCount) * 100;

        mClassEngraveLabels[i]->setText(statisticEngraveText
                                       .arg(classEngraves[i])
                                       .arg(ratio, 0, 'f', 2));
    }
}

void StatisticArmory::enableInput(bool enable)
{
    pClassSelector->setEnabled(enable);
    pSearchButton->setEnabled(enable);
}

void StatisticArmory::refresh()
{

}

StatisticArmory *StatisticArmory::getInstance()
{
    if (pInstance == nullptr)
        pInstance = new StatisticArmory();

    return pInstance;
}

void StatisticArmory::destoryInstance()
{
    if (pInstance == nullptr)
        return;

    delete pInstance;
    pInstance = nullptr;
}
