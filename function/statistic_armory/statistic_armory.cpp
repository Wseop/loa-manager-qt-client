#include "statistic_armory.h"
#include "ui_statistic_armory.h"
#include "ui/widget_manager.h"
#include "ui/font_manager.h"
#include "api/api_manager.h"

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
    mParseMainKeys({"abilityCounts", "setCounts", "engraveCounts", "elixirCounts"}),
    mParseSubKeys({"ability", "set", "engrave", "elixir"}),
    mLayoutKeys({"특성", "세트", "각인", "엘릭서"}),
    pClassSelector(nullptr)
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
            this, &StatisticArmory::searchProfile);
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

QLabel *StatisticArmory::createInfoLabel()
{
    return WidgetManager::createLabel("1% 미만일 경우\n결과에서 제외됩니다.", 10, 200, 50);
}

void StatisticArmory::initializeClassEngraveLayout()
{
    ui->hLayoutEngrave->setAlignment(Qt::AlignHCenter);

    for (int i = 0; i < 2; i++)
    {
        ui->hLayoutEngrave->addWidget(createClassEngraveButton());
    }
}

QPushButton *StatisticArmory::createClassEngraveButton()
{
    QPushButton *pClassEngraveButton = WidgetManager::createPushButton("-", 10, 125);

    mClassEngraveButtons << pClassEngraveButton;

    connect(pClassEngraveButton, &QPushButton::released,
            this, [&, pClassEngraveButton]()
    {
        const QString classEngrave = pClassEngraveButton->text().split("\n").front();

        for (QPushButton *pButton : mClassEngraveButtons)
            pButton->setStyleSheet("");

        pClassEngraveButton->setStyleSheet("QPushButton {"
                                           "border: 2px solid blue;"
                                           "border-radius: 5px; "
                                           "}");

        enableInput(false);
        searchStatistic(classEngrave);
    });

    return pClassEngraveButton;
}

void StatisticArmory::initializeResultLayout()
{
    ui->vLayoutResult->setAlignment(Qt::AlignTop);

    for (const QString &layoutKey : mLayoutKeys)
    {
        if (layoutKey == "각인")
        {
            for (int i = 3; i > 0; i--)
            {
                ui->vLayoutResult->addWidget(
                    createCategoryLabel(layoutKey + QString("(Lv.%1)").arg(i)));
                ui->vLayoutResult->addLayout(
                    createResultLayout(layoutKey + QString("(Lv.%1)").arg(i)));
            }
        }
        else
        {
            ui->vLayoutResult->addWidget(createCategoryLabel(layoutKey));
            ui->vLayoutResult->addLayout(createResultLayout(layoutKey));
        }
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

QVBoxLayout *StatisticArmory::createResultLayout(const QString &category)
{
    QVBoxLayout* pResultLayout = new QVBoxLayout();

    mResultLayoutMap[category] = pResultLayout;

    return pResultLayout;
}

void StatisticArmory::searchProfile(const QString &className)
{
    enableInput(false);

    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished,
            pNetworkManager, &QNetworkAccessManager::deleteLater);
    connect(pNetworkManager, &QNetworkAccessManager::finished,
            this, &StatisticArmory::parseProfile);

    ApiManager::getInstance()->getStatistics(pNetworkManager, Statistics::Profile, {className});
}

void StatisticArmory::parseProfile(QNetworkReply *pReply)
{
    if (!ApiManager::getInstance()->handleStatusCode(pReply))
    {
        enableInput(true);
        return;
    }
    mClassEngraves.clear();

    const QJsonObject response = QJsonDocument::fromJson(pReply->readAll()).object();
    const double count = response.find("count")->toDouble();
    const QJsonArray &classEngraveCounts = response.find("classEngraveCounts")->toArray();

    for (int i = 0; i < classEngraveCounts.size(); i++)
    {
        const double ratio = classEngraveCounts[i].toObject().find("count")->toInt() / count * 100;
        const QString &classEngrave = classEngraveCounts[i].toObject().find("classEngrave")->toString();

        mClassEngraves << classEngrave;
        mClassEngraveButtons[i]->setText(QString("%1\n(%2%)").arg(classEngrave).arg(ratio, 0, 'f', 2));
    }

    enableInput(true);
}

void StatisticArmory::searchStatistic(const QString &classEngrave)
{
    clearResult();

    QList<Statistics> categories = {
        Statistics::Ability,
        Statistics::Elixir,
        Statistics::Set,
        Statistics::Engrave
    };

    for (auto category : categories)
    {
        if (category == Statistics::Engrave)
        {
            for (int i = 1; i <= 3; i++)
            {
                QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
                connect(pNetworkManager, &QNetworkAccessManager::finished,
                        pNetworkManager, &QNetworkAccessManager::deleteLater);
                connect(pNetworkManager, &QNetworkAccessManager::finished,
                        this, [&, category, i](QNetworkReply *pReply)
                        {
                            mResponseCount++;

                            if (ApiManager::getInstance()->handleStatusCode(pReply))
                                parseStatistic(category, QString::number(i), pReply);
                        });

                ApiManager::getInstance()->getStatistics(pNetworkManager, category, {QString::number(i), classEngrave});
            }
        }
        else
        {
            QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
            connect(pNetworkManager, &QNetworkAccessManager::finished,
                    pNetworkManager, &QNetworkAccessManager::deleteLater);
            connect(pNetworkManager, &QNetworkAccessManager::finished,
                    this, [&, category](QNetworkReply *pReply)
                    {
                        mResponseCount++;

                        if (ApiManager::getInstance()->handleStatusCode(pReply))
                            parseStatistic(category, "", pReply);
                    });

            ApiManager::getInstance()->getStatistics(pNetworkManager, category, {classEngrave});
        }
    }
}

void StatisticArmory::parseStatistic(Statistics category, QString engraveLevel, QNetworkReply *pReply)
{
    QString mainKey = mParseMainKeys[static_cast<int>(category) - static_cast<int>(Statistics::Ability)];
    QString subKey = mParseSubKeys[static_cast<int>(category) - static_cast<int>(Statistics::Ability)];

    const QJsonObject response = QJsonDocument::fromJson(pReply->readAll()).object();
    const double count = response.find("count")->toDouble();
    const QJsonArray &datas = response.find(mainKey)->toArray();

    for (auto iter = datas.constBegin(); iter != datas.constEnd(); iter++)
    {
        const QString &value = iter->toObject().find(subKey)->toString();
        const double ratio = iter->toObject().find("count")->toInt() / count * 100;

        if (ratio >= 1)
            setResult(category, engraveLevel, value, ratio);
    }

    if (mResponseCount >= 4)
        enableInput(true);
}

void StatisticArmory::clearResult()
{
    for (QLabel *pLabel : mResultLabels) {
        delete pLabel;
    }

    mResultLabels.clear();
    mResponseCount = 0;
}

void StatisticArmory::setResult(Statistics category, const QString &engraveLevel, const QString &value, const double &ratio)
{
    QString layoutKey = mLayoutKeys[static_cast<int>(category) - static_cast<int>(Statistics::Ability)];

    if (category == Statistics::Engrave)
        layoutKey += QString("(Lv.%1)").arg(engraveLevel);

    QLabel *pResultLabel = WidgetManager::createLabel(
        QString("%1 (%2%)").arg(value).arg(ratio, 0, 'f', 2), 14);

    if (ratio >= 20)
        pResultLabel->setStyleSheet("QLabel { color: blue }");

    mResultLayoutMap[layoutKey]->addWidget(pResultLabel);
    mResultLabels << pResultLabel;
}

void StatisticArmory::enableInput(bool enable)
{
    pClassSelector->setEnabled(enable);

    for (QPushButton *pButton : mClassEngraveButtons)
        pButton->setEnabled(enable);
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
