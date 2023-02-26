#include "tripod_quotation.h"
#include "ui_tripod_quotation.h"
#include "ui/font_manager.h"
#include "ui/class_selector.h"
#include "function/quotation/tripod/skill_info_widget.h"
#include "resource/resource_manager.h"

#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QIntValidator>

TripodQuotation* TripodQuotation::m_pInstance = nullptr;

TripodQuotation::TripodQuotation() :
    ui(new Ui::TripodQuotation),
    m_pClassSelector(new ClassSelector()),
    m_pPriceValidator(new QIntValidator())
{
    ui->setupUi(this);
    ui->lePrice->setValidator(m_pPriceValidator);
    ui->hLayoutInput->setAlignment(Qt::AlignHCenter);
    ui->vLayoutResult->setAlignment(Qt::AlignHCenter);

    loadSkillData();
    initFont();
    initClassSelector();
    initPriceEmphasis();
}

TripodQuotation::~TripodQuotation()
{
    delete m_pClassSelector;
    delete m_pPriceValidator;
    clear();
    delete ui;
}

void TripodQuotation::loadSkillData()
{
    const QJsonObject json = ResourceManager::getInstance()->loadJson("skill");

    const QJsonArray& jsonArrSkill = json.find("Skill")->toArray();
    for (const QJsonValue& value : jsonArrSkill)
    {
        const QJsonObject& object = value.toObject();

        const QString& cls = object.find("Class")->toString();

        const QJsonArray& skills = object.find("Skills")->toArray();
        for (const QJsonValue& value : skills)
        {
            const QJsonObject& skill = value.toObject();

            const int& skillValue = skill.find("Value")->toInt();
            m_classToSkills[cls].append({
                                            skill.find("Text")->toString(),
                                            skillValue,
                                            skill.find("IconPath")->toString()
                                        });

            const QJsonArray& tripods = skill.find("Tripods")->toArray();
            for (const QJsonValue& value : tripods)
            {
                const QJsonObject& tripod = value.toObject();
                m_skillValueToTripods[skillValue].append({
                                                       tripod.find("Text")->toString(),
                                                       tripod.find("Value")->toInt(),
                                                       tripod.find("IconIndex")->toInt()
                                                   });
            }
        }
    }
}

void TripodQuotation::initFont()
{
    FontManager* pFontManager = FontManager::getInstance();
    QFont nanumBold10 = pFontManager->getFont(FontFamily::NanumSquareNeoBold, 10);

    ui->pbSelectClass->setFont(nanumBold10);
    ui->groupPrice->setFont(nanumBold10);
    ui->lePrice->setFont(nanumBold10);
    ui->lbOver->setFont(nanumBold10);
    ui->lbExplain->setFont(nanumBold10);
}

void TripodQuotation::initClassSelector()
{
    connect(ui->pbSelectClass, &QPushButton::released, this, [&](){
        m_pClassSelector->show();
    });

    const QList<QPushButton*>& classSelectBtns = m_pClassSelector->getButtons();
    for (QPushButton* pButton : classSelectBtns)
    {
        connect(pButton, &QPushButton::released, this, [&, pButton](){
            QString oldClass = ui->pbSelectClass->text();
            QString newClass = pButton->text();
            if (oldClass != newClass)
            {
                ui->pbSelectClass->setText(newClass);
                clear();
                addSkillInfoWidgets(newClass);
            }
            m_pClassSelector->hide();
        });
    }
}

void TripodQuotation::initPriceEmphasis()
{
    connect(ui->lePrice, &QLineEdit::returnPressed, this, [&](){
        for (SkillInfoWidget* pSkillInfoWidget : m_skillInfoWidgets)
            pSkillInfoWidget->setPriceEmphasis(ui->lePrice->text().toInt());
    });
}

void TripodQuotation::clear()
{
    ui->lePrice->clear();

    for (SkillInfoWidget* pSkillInfoWidget : m_skillInfoWidgets)
        delete pSkillInfoWidget;
    m_skillInfoWidgets.clear();
}

void TripodQuotation::addSkillInfoWidgets(QString cls)
{
    const QList<SkillInfo>& skillInfos = m_classToSkills[cls];
    for (const SkillInfo& skillInfo : skillInfos)
    {
        SkillInfoWidget* pSkillInfoWidget = new SkillInfoWidget(this, skillInfo, m_skillValueToTripods[skillInfo.auctionCode]);
        m_skillInfoWidgets.append(pSkillInfoWidget);
        ui->vLayoutResult->addWidget(pSkillInfoWidget);
    }
}

TripodQuotation* TripodQuotation::getInstance()
{
    if (m_pInstance == nullptr)
        m_pInstance = new TripodQuotation();
    return m_pInstance;
}

void TripodQuotation::destroyInstance()
{
    if (m_pInstance == nullptr)
        return;
    delete m_pInstance;
    m_pInstance = nullptr;
}
