#include "character_ranking.h"
#include "ui_character_ranking.h"
#include "ui/font_manager.h"
#include "ui/widget_manager.h"
#include "ui/class_selector.h"
#include "game_data/character/profile/enum/class.h"
#include "db/db_manager.h"
#include "function/character_search/character_search.h"
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QThread>

CharacterRanking* CharacterRanking::m_pInstance = nullptr;

CharacterRanking::CharacterRanking() :
    ui(new Ui::CharacterRanking),
    m_bLoaded(false),
    m_pClassSelector(new ClassSelector()),
    m_renderCount(0)
{
    ui->setupUi(this);

    setAlignments();
    setFonts();
    initConnects();

    emit refresh();
}

CharacterRanking::~CharacterRanking()
{
    delete ui;
}

void CharacterRanking::setAlignments()
{
    ui->vLayoutCharacterRanking->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    ui->vLayoutRankingData->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
}

void CharacterRanking::setFonts()
{
    FontManager* pFontManager = FontManager::getInstance();
    QFont nanumBold10 = pFontManager->getFont(FontFamily::NanumSquareNeoBold, 10);

    ui->pbSelectAllClass->setFont(nanumBold10);
    ui->pbSelectClass->setFont(nanumBold10);
    ui->lbSelectedClass->setFont(nanumBold10);
    ui->lbSelectedItemLevel->setFont(nanumBold10);
    ui->pbRenderMore->setFont(nanumBold10);
}

void CharacterRanking::initConnects()
{
    connect(this, &CharacterRanking::refresh, this, &CharacterRanking::clearRankingData);
    connect(this, &CharacterRanking::refresh, this, [&](){
        setEnabled(false);

        QThread* pThread = QThread::create(tGetRankingData);
        connect(pThread, &QThread::finished, this, &CharacterRanking::updateUI);
        connect(pThread, &QThread::finished, this, [&](){
            setEnabled(true);
        });
        connect(pThread, &QThread::finished, pThread, &QThread::deleteLater);
        pThread->start();
    });

    connect(ui->hSliderItemLevel, &QSlider::valueChanged, this, [&](int value){
        clearRankingData();
        updateUI();
    });
    connect(ui->hSliderItemLevel, &QSlider::sliderMoved, this, [&](int value){
        ui->lbSelectedItemLevel->setText(QString("아이템 레벨\n%1").arg(value));
    });
    connect(ui->pbSelectAllClass, &QPushButton::released, this, [&](){
        QString oldClass = ui->lbSelectedClass->text();
        QString newClass = ui->pbSelectAllClass->text();
        if (oldClass != newClass)
        {
            ui->lbSelectedClass->setText(newClass);
            emit refresh();
        }
    });
    connect(ui->pbSelectClass, &QPushButton::released, this, [&](){
        m_pClassSelector->show();
    });

    // init ClassSelector's button action
    QList<QPushButton*>& pClassButtons = m_pClassSelector->getButtons();
    for (QPushButton* pClassButton : pClassButtons)
    {
        connect(pClassButton, &QPushButton::released, this, [&, pClassButton](){
            QString oldClass = ui->lbSelectedClass->text();
            QString newClass = pClassButton->text();
            if (oldClass != newClass)
            {
                ui->lbSelectedClass->setText(newClass);
                emit refresh();
            }
            m_pClassSelector->hide();
        });
    }

    connect(ui->pbRenderMore, &QPushButton::released, this, &CharacterRanking::updateUI);
}

void CharacterRanking::addCharacterData(int index, QString server, QString cls, QString name, double itemLevel)
{
    const int WIDGET_WIDTH = 150;
    const int WIDGET_HEIGHT = 25;

    QHBoxLayout* pHLayout = new QHBoxLayout();
    ui->vLayoutRankingData->addLayout(pHLayout);
    m_hLayouts.append(pHLayout);

    QLabel* pLabelIndex = WidgetManager::createLabel(QString::number(index), 10, "", WIDGET_WIDTH, WIDGET_HEIGHT);
    pHLayout->addWidget(pLabelIndex);
    m_labels.append(pLabelIndex);

    QLabel* pLabelServer = WidgetManager::createLabel(server, 10, "", WIDGET_WIDTH, WIDGET_HEIGHT);
    pHLayout->addWidget(pLabelServer);
    m_labels.append(pLabelServer);

    QLabel* pLabelClass = WidgetManager::createLabel(cls, 10, "", WIDGET_WIDTH, WIDGET_HEIGHT);
    pHLayout->addWidget(pLabelClass);
    m_labels.append(pLabelClass);

    QPushButton* pButtonName = WidgetManager::createPushButton(name, 10, WIDGET_WIDTH, WIDGET_HEIGHT);
    pHLayout->addWidget(pButtonName);
    m_buttons.append(pButtonName);
    connect(pButtonName, &QPushButton::pressed, this, [&, pButtonName](){
        CharacterSearch::getInstance()->changeCharacter(pButtonName->text());
        this->hide();
        CharacterSearch::getInstance()->show();
    });

    QLabel* pLabelItemLevel = WidgetManager::createLabel(QString::number(itemLevel), 10, "", WIDGET_WIDTH, WIDGET_HEIGHT);
    pHLayout->addWidget(pLabelItemLevel);
    m_labels.append(pLabelItemLevel);
}

void CharacterRanking::updateUI()
{
    const int NUMBER_PER_RENDER = 100;

    for (int i = 0; (i < NUMBER_PER_RENDER) && (m_renderCount < m_jArrRankingData.size()); i++)
    {
        const QJsonObject& jObjData = m_jArrRankingData[m_renderCount].toObject();
        QString server = jObjData.find("Server")->toString();
        QString cls = jObjData.find("Class")->toString();
        QString name = jObjData.find("Name")->toString();
        double itemLevel = jObjData.find("Level")->toDouble();

        if (static_cast<int>(itemLevel) >= ui->hSliderItemLevel->value())
            addCharacterData(++m_renderCount, server, cls, name, itemLevel);
    }
}

void CharacterRanking::clearRankingData()
{
    m_renderCount = 0;

    for (QLabel* pLabel : m_labels)
        delete pLabel;
    m_labels.clear();

    for (QPushButton* pButton : m_buttons)
        delete pButton;
    m_buttons.clear();

    for (QHBoxLayout* pLayout : m_hLayouts)
        delete pLayout;
    m_hLayouts.clear();
}

void CharacterRanking::setEnable(bool enable)
{
    ui->pbSelectAllClass->setEnabled(enable);
    ui->pbSelectClass->setEnabled(enable);
    ui->pbRenderMore->setEnabled(enable);
}

void CharacterRanking::tGetRankingData()
{
    CharacterRanking* pInstance = CharacterRanking::getInstance();
    pInstance->m_bLoaded = true;

    DbManager* pDbManager = DbManager::getInstance();
    bsoncxx::builder::stream::document filter{};

    Class cls = strToClass(pInstance->ui->lbSelectedClass->text());
    if (cls != Class::Size)
        filter << "Class" << classToStr(cls).toStdString();

    pDbManager->lock();
    pInstance->m_jArrRankingData = pDbManager->findDocuments(Collection::Character, SortOrder::Desc, "Level", filter.extract());
    pDbManager->unlock();
}

CharacterRanking* CharacterRanking::getInstance()
{
    if (m_pInstance == nullptr)
        m_pInstance = new CharacterRanking();
    return m_pInstance;
}

void CharacterRanking::destroyInstance()
{
    if (m_pInstance == nullptr)
        return;
    delete m_pInstance;
    m_pInstance = nullptr;
}

bool CharacterRanking::loaded()
{
    return m_bLoaded;
}
