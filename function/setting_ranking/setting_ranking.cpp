#include "setting_ranking.h"
#include "ui_setting_ranking.h"
#include "ui/font_manager.h"
#include "ui/class_selector.h"
#include "function/setting_ranking/setting_widget.h"
#include "db/db_manager.h"
#include <QThread>
#include <QMultiMap>
#include <functional>

SettingRanking* SettingRanking::m_pInstance = nullptr;

SettingRanking::SettingRanking() :
    ui(new Ui::SettingRanking),
    m_pClassSelector(new ClassSelector()),
    m_total(0),
    m_renderCount(0)
{
    ui->setupUi(this);

    initConnects();
    setAlignments();
    setFonts();
}

SettingRanking::~SettingRanking()
{
    delete ui;
}

void SettingRanking::initConnects()
{
    connect(ui->pbSelectClass, &QPushButton::released, this, [&](){
        m_pClassSelector->show();
    });

    QList<QPushButton*> classButtons = m_pClassSelector->getButtons();
    for (const QPushButton* pButton : classButtons)
    {
        connect(pButton, &QPushButton::released, this, [&, pButton](){
            QString oldClass = ui->pbSelectClass->text();
            QString newClass = pButton->text();
            if (oldClass != newClass)
            {
                ui->pbSelectClass->setEnabled(false);
                ui->pbSelectClass->setText(newClass);
                clear();
                ui->lbInfo->setText("데이터 불러오는 중...");

                QThread* pThread = QThread::create(tLoadSettingData, newClass);
                connect(pThread, &QThread::finished, pThread, &QThread::deleteLater);
                pThread->start();
            }
            m_pClassSelector->hide();
        });
    }

    connect(this, &SettingRanking::refresh, this, &SettingRanking::updateUI);
    connect(ui->pbShowMore, &QPushButton::released, this, &SettingRanking::updateUI);
}

void SettingRanking::setAlignments()
{
    ui->vLayoutSettingRanking->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    ui->hLayoutTop->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    ui->vLayoutSettingData->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
}

void SettingRanking::setFonts()
{
    FontManager* pFontManager = FontManager::getInstance();
    QFont nanumBold10 = pFontManager->getFont(FontFamily::NanumSquareNeoBold, 10);

    ui->pbSelectClass->setFont(nanumBold10);
    ui->lbInfo->setFont(nanumBold10);
    ui->pbShowMore->setFont(nanumBold10);
}

void SettingRanking::updateUI()
{
    const int NUMBER_PER_RENDER = 100;

    ui->lbInfo->setText(QString("등록된 캐릭터 수 : %1").arg(m_total));

    for (int i = 0; (i < NUMBER_PER_RENDER) && (m_renderCount < m_settingCodeCounts.size()); i++)
    {
        const QPair<SettingCode, int>& settingCodeCount = m_settingCodeCounts[m_renderCount++];
        const SettingCode& settingCode = settingCodeCount.first;
        const QList<CharacterInfo>& characterInfos = m_settingCodeToCharacterInfos.values(settingCode);
        SettingWidget* pSettingWidget = new SettingWidget(this, m_renderCount, settingCode, characterInfos, m_total);
        m_settingWidgets.append(pSettingWidget);
        ui->vLayoutSettingData->addWidget(pSettingWidget);
    }

    ui->pbSelectClass->setEnabled(true);
}

void SettingRanking::clear()
{
    m_settingCodeToCharacterInfos.clear();
    m_settingCodeCounts.clear();
    m_total = 0;
    m_renderCount = 0;
    for (SettingWidget* pSettingWidget : m_settingWidgets)
        delete pSettingWidget;
    m_settingWidgets.clear();
}

void SettingRanking::tLoadSettingData(QString cls)
{
    SettingRanking* pInstance = SettingRanking::getInstance();
    DbManager* pDbManager = DbManager::getInstance();
    bsoncxx::builder::stream::document filter{};
    filter << "Class" << cls.toStdString();

    // load data from db
    pDbManager->lock();
    QJsonArray jArrSettingData = pDbManager->findDocuments(Collection::Setting, SortOrder::Desc, "Level", filter.extract());
    pDbManager->unlock();

    // process data (init m_settingCodeToCharacterInfos)
    QMultiMap<SettingCode, CharacterInfo>& settingCodeToCharacterInfos = pInstance->m_settingCodeToCharacterInfos;
    for (const QJsonValue& jValSettingData : jArrSettingData)
    {
        const QJsonObject& jObjSettingData = jValSettingData.toObject();
        CharacterInfo info = {
            jObjSettingData.find("Name")->toString(),
            jObjSettingData.find("Class")->toString(),
            jObjSettingData.find("Level")->toDouble()
        };
        SettingCode settingCode = jObjSettingData.find("SettingCode")->toString();

        settingCodeToCharacterInfos.insert(settingCode, info);
    }

    // process data (init m_settingCodeCounts)
    QList<QPair<SettingCode, int>>& settingCodeCounts = pInstance->m_settingCodeCounts;
    int& total = pInstance->m_total;
    const QStringList& settingCodes = settingCodeToCharacterInfos.uniqueKeys();
    for (const QString& settingCode : settingCodes)
    {
        int count = settingCodeToCharacterInfos.values(settingCode).size();
        settingCodeCounts.append({settingCode, count});
        total += count;
    }
    std::sort(settingCodeCounts.begin(), settingCodeCounts.end(), [&](auto& a, auto& b){
        return a.second > b.second;
    });

    emit SettingRanking::getInstance()->refresh();
}

SettingRanking* SettingRanking::getInstance()
{
    if (m_pInstance == nullptr)
        m_pInstance = new SettingRanking();
    return m_pInstance;
}

void SettingRanking::destroyInstance()
{
    if (m_pInstance == nullptr)
        return;
    delete m_pInstance;
    m_pInstance = nullptr;
}
