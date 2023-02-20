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
    m_skillInfoWidgets(0),
    m_pPriceValidator(new QIntValidator())
{
    ui->setupUi(this);
    ui->lePrice->setValidator(m_pPriceValidator);

    loadSkillData();
    setFonts();
    setAlignments();
    initConnects();
}

TripodQuotation::~TripodQuotation()
{
    delete ui;
}

void TripodQuotation::loadSkillData()
{
    const QJsonObject json = ResourceManager::getInstance()->loadJson("skill");

    const QJsonArray& arrData = json.find("Skill")->toArray();
    for (const QJsonValue& valueData : arrData)
    {
        const QJsonObject& objData = valueData.toObject();
        QString cls = objData.find("Class")->toString();
        const QJsonArray& arrSkill = objData.find("Skills")->toArray();

        for (const QJsonValue& valueSkill : arrSkill)
        {
            const QJsonObject& objSkill = valueSkill.toObject();
            const int& skillValue = objSkill.find("Value")->toInt();
            m_classToSkills[cls].append({
                                            objSkill.find("Text")->toString(),
                                            skillValue,
                                            objSkill.find("IconPath")->toString()
                                        });

            const QJsonArray& arrTripod = objSkill.find("Tripods")->toArray();
            for (const QJsonValue& valueTripod : arrTripod)
            {
                const QJsonObject& objTripod = valueTripod.toObject();
                m_skillValueToTripods[skillValue].append({
                                                       objTripod.find("Text")->toString(),
                                                       objTripod.find("Value")->toInt(),
                                                       objTripod.find("IconIndex")->toInt()
                                                   });
            }
        }
    }
}

void TripodQuotation::setFonts()
{
    FontManager* pFontManager = FontManager::getInstance();
    QFont nanumBold10 = pFontManager->getFont(FontFamily::NanumSquareNeoBold, 10);

    ui->pbSelectClass->setFont(nanumBold10);
    ui->groupPrice->setFont(nanumBold10);
    ui->lePrice->setFont(nanumBold10);
    ui->lbOver->setFont(nanumBold10);
    ui->lbExplain->setFont(nanumBold10);
}

void TripodQuotation::setAlignments()
{
    ui->hLayoutInput->setAlignment(Qt::AlignHCenter);
    ui->vLayoutResult->setAlignment(Qt::AlignHCenter);
}

void TripodQuotation::initConnects()
{
    connect(ui->pbSelectClass, &QPushButton::released, this, [&](){
        m_pClassSelector->show();
    });

    QList<QPushButton*> classSelectBtns = m_pClassSelector->getButtons();
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
