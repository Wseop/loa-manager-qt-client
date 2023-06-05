#include "statistic_armory.h"
#include "ui_statistic_armory.h"
#include "ui/widget_manager.h"
#include "ui/font_manager.h"
#include "api/api_manager.h"
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
    pSearchButton(nullptr),
    pTotalLabel(nullptr)
{
    ui->setupUi(this);
    ui->vLayoutMain->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    initializeClassLayout();
    initializeTotalLayout();
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

void StatisticArmory::initializeTotalLayout()
{
    ui->hLayoutTotal->addWidget(createTotalLabel());
}

QLabel *StatisticArmory::createTotalLabel()
{
    pTotalLabel = WidgetManager::createLabel("", 14, 200, 30);

    return pTotalLabel;
}

void StatisticArmory::initializeClassEngraveLayout()
{
    for (int i = 0; i < 3; i++) {
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

    for (int i = 0; i < 3; i++) {
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

    QNetworkAccessManager *pStatisticLoader = new QNetworkAccessManager;

    connect(pStatisticLoader, &QNetworkAccessManager::finished,
            pStatisticLoader, &QNetworkAccessManager::deleteLater);
    connect(pStatisticLoader, &QNetworkAccessManager::finished,
            this, &StatisticArmory::parseStatistic);

    ApiManager::getInstance()->getStatistics(
        pStatisticLoader, Statistics::SettingsArmory, mSelectedClassName);
}

void StatisticArmory::parseStatistic(QNetworkReply *pReply)
{
    if (!ApiManager::getInstance()->handleStatusCode(pReply)) {
        enableInput(true);
        return;
    }

    QStringList classEngraves = EngraveManager::getInstance()
                                           ->getClassEngraves(mSelectedClassName);
    classEngraves << "쌍직각";

    SettingsArmory statisticData;
    QJsonObject response = QJsonDocument::fromJson(pReply->readAll()).object();

    statisticData.count = response.find("count")->toInt();

    for (const QString &classEngrave : classEngraves) {
        const QJsonObject &data = response.find(classEngrave)->toObject();
        const QJsonObject &abilities = data.find("abilities")->toObject();
        const QJsonObject &itemSets = data.find("itemSets")->toObject();
        const QJsonObject &elixirs = data.find("elixirs")->toObject();
        const QJsonArray &engraves = data.find("engraves")->toArray();

        statisticData.settings[classEngrave].count = data.find("count")->toInt();

        for (auto iter = abilities.constBegin(); iter != abilities.constEnd(); iter++) {
            if (iter.value().toInt() >= 10 && iter.key() != "") {
                statisticData.settings[classEngrave].abilities.append(
                    {iter.key(), iter.value().toInt()}
                );
            }
        }

        for (auto iter = itemSets.constBegin(); iter != itemSets.constEnd(); iter++) {
            if (iter.value().toInt() >= 10 && iter.key() != "") {
                statisticData.settings[classEngrave].itemSets.append(
                    {iter.key(), iter.value().toInt()}
                );
            }
        }

        for (auto iter = elixirs.constBegin(); iter != elixirs.constEnd(); iter++) {
            if (iter.value().toInt() >= 10 && iter.key() != "") {
                statisticData.settings[classEngrave].elixirs.append(
                    {iter.key(), iter.value().toInt()}
                );
            }
        }

        for (int i = 0; i < 3; i++) {
            const QJsonObject &engrave = engraves[i].toObject();

            for (auto iter = engrave.constBegin(); iter != engrave.constEnd(); iter++) {
                if (iter.value().toInt() >= 10 && iter.key() != "") {
                    statisticData.settings[classEngrave].engraves[i].append(
                        {iter.key(), iter.value().toInt()}
                    );
                }
            }
        }
    }

    mStatisticData = statisticData;

    // 데이터 내림차순 정렬
    for (int i = 0; i < classEngraves.size(); i++) {
        auto &abilities = mStatisticData.settings[classEngraves[i]].abilities;
        auto &itemSets = mStatisticData.settings[classEngraves[i]].itemSets;
        auto &elixirs = mStatisticData.settings[classEngraves[i]].elixirs;
        auto &engraves = mStatisticData.settings[classEngraves[i]].engraves;

        std::sort(abilities.begin(), abilities.end(), [&](auto a, auto b){
            return a.second > b.second;
        });
        std::sort(itemSets.begin(), itemSets.end(), [&](auto a, auto b){
            return a.second > b.second;
        });
        std::sort(elixirs.begin(), elixirs.end(), [&](auto a, auto b){
            return a.second > b.second;
        });

        for (int i = 0; i < 3; i++) {
            std::sort(engraves[i].begin(), engraves[i].end(), [&](auto a, auto b){
                return a.second > b.second;
            });
        }
    }

    clearResult();

    setTotalCount();
    setClassEngraveCount(classEngraves);
    setAbilityCount(classEngraves);
    setEngraveCount(classEngraves);
    setItemSetCount(classEngraves);
    setElixirCount(classEngraves);

    enableInput(true);
}

void StatisticArmory::clearResult()
{
    for (QLabel *pLabel : mResultLabels) {
        delete pLabel;
    }

    mResultLabels.clear();
}

void StatisticArmory::setTotalCount()
{
    pTotalLabel->setText(QString("검색된 캐릭터 : %1").arg(mStatisticData.count));
}

void StatisticArmory::setClassEngraveCount(const QStringList &classEngraves)
{
    const QString statisticEngraveText = "%1\n(%2%)";

    for (int i = 0; i < classEngraves.size(); i++) {
        double ratio = mStatisticData.settings[classEngraves[i]].count /
                       static_cast<double>(mStatisticData.count) * 100;

        mClassEngraveLabels[i]->setText(statisticEngraveText
                                       .arg(classEngraves[i])
                                       .arg(ratio, 0, 'f', 2));
    }
}

void StatisticArmory::setAbilityCount(const QStringList &classEngraves)
{
    const QString text = "%1 (%2%)";

    for (int i = 0; i < classEngraves.size(); i++) {
        const auto &abilities = mStatisticData.settings[classEngraves[i]].abilities;

        for (const auto &ability : abilities) {
            double ratio = ability.second /
                           static_cast<double>(
                               mStatisticData.settings[classEngraves[i]].count) * 100;

            QLabel *pLabel = WidgetManager::createLabel(
                text.arg(ability.first).arg(ratio, 0, 'f', 2), 14);
            pLabel->setMaximumWidth(QWIDGETSIZE_MAX);

            if (ratio >= 20) {
                pLabel->setStyleSheet("QLabel { color: blue }");
            }

            mCategoryLayouts["특성"][i]->addWidget(pLabel);
            mResultLabels << pLabel;
        }

        // 데이터가 없을 경우 layout 맞춰주기 위해 dummy label 추가
        if (abilities.size() == 0) {
            QLabel *pLabel = WidgetManager::createLabel("-");
            pLabel->setMaximumWidth(QWIDGETSIZE_MAX);

            mCategoryLayouts["특성"][i]->addWidget(pLabel);
            mResultLabels << pLabel;
        }
    }
}

void StatisticArmory::setEngraveCount(const QStringList &classEngraves)
{
    const QString text = "%1 (%2%)";

    for (int i = 0; i < classEngraves.size(); i++) {
        for (int j = 0; j < 3; j++) {
            const auto &engraves = mStatisticData.settings[classEngraves[i]]
                                       .engraves[j];

            for (const auto &engrave : engraves) {
                double ratio = engrave.second /
                               static_cast<double>(
                                   mStatisticData.settings[classEngraves[i]].count) * 100;

                QLabel *pLabel = WidgetManager::createLabel(
                    text.arg(engrave.first).arg(ratio, 0, 'f', 2), 14);
                pLabel->setMaximumWidth(QWIDGETSIZE_MAX);

                if (ratio >= 20) {
                    pLabel->setStyleSheet("QLabel { color: blue }");
                }

                mCategoryLayouts[QString("각인(Lv.%1)").arg(j + 1)][i]
                    ->addWidget(pLabel);
                mResultLabels << pLabel;
            }

            // 데이터가 없을 경우 layout 맞춰주기 위해 dummy label 추가
            if (engraves.size() == 0) {
                QLabel *pLabel = WidgetManager::createLabel("-");
                pLabel->setMaximumWidth(QWIDGETSIZE_MAX);

                mCategoryLayouts[QString("각인(Lv.%1)").arg(j + 1)][i]
                    ->addWidget(pLabel);
                mResultLabels << pLabel;
            }
        }
    }
}

void StatisticArmory::setItemSetCount(const QStringList &classEngraves)
{
    const QString text = "%1 (%2%)";

    for (int i = 0; i < classEngraves.size(); i++) {
        const auto &itemSets = mStatisticData.settings[classEngraves[i]].itemSets;

        for (const auto &itemSet : itemSets) {
            double ratio = itemSet.second /
                           static_cast<double>(
                               mStatisticData.settings[classEngraves[i]].count) * 100;

            QLabel *pLabel = WidgetManager::createLabel(
                text.arg(itemSet.first).arg(ratio, 0, 'f', 2), 14);
            pLabel->setMaximumWidth(QWIDGETSIZE_MAX);

            if (ratio >= 20) {
                pLabel->setStyleSheet("QLabel { color: blue }");
            }

            mCategoryLayouts["세트효과"][i]->addWidget(pLabel);
            mResultLabels << pLabel;
        }

        // 데이터가 없을 경우 layout 맞춰주기 위해 dummy label 추가
        if (itemSets.size() == 0) {
            QLabel *pLabel = WidgetManager::createLabel("-");
            pLabel->setMaximumWidth(QWIDGETSIZE_MAX);

            mCategoryLayouts["세트효과"][i]->addWidget(pLabel);
            mResultLabels << pLabel;
        }
    }
}

void StatisticArmory::setElixirCount(const QStringList &classEngraves)
{
    const QString text = "%1 (%2%)";

    for (int i = 0; i < classEngraves.size(); i++) {
        const auto &elixirs = mStatisticData.settings[classEngraves[i]].elixirs;

        for (const auto &elixir : elixirs) {
            double ratio = elixir.second /
                           static_cast<double>(
                               mStatisticData.settings[classEngraves[i]].count) * 100;

            QLabel *pLabel = WidgetManager::createLabel(
                text.arg(elixir.first).arg(ratio, 0, 'f', 2), 14);
            pLabel->setMaximumWidth(QWIDGETSIZE_MAX);

            if (ratio >= 20) {
                pLabel->setStyleSheet("QLabel { color: blue }");
            }

            mCategoryLayouts["엘릭서"][i]->addWidget(pLabel);
            mResultLabels << pLabel;
        }

        // 데이터가 없을 경우 layout 맞춰주기 위해 dummy label 추가
        if (elixirs.size() == 0) {
            QLabel *pLabel = WidgetManager::createLabel("-");
            pLabel->setMaximumWidth(QWIDGETSIZE_MAX);

            mCategoryLayouts["엘릭서"][i]->addWidget(pLabel);
            mResultLabels << pLabel;
        }
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
