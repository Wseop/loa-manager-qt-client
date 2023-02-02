#include "tripod_quotation.h"
#include "ui_tripod_quotation.h"
#include "ui/font_manager.h"
#include "ui/class_selector.h"
#include "function/quotation/ui/skill_info_widget.h"
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

TripodQuotation* TripodQuotation::m_pInstance = nullptr;

TripodQuotation::TripodQuotation() :
    ui(new Ui::TripodQuotation),
    m_pClassSelector(new ClassSelector()),
    m_skillInfoWidgets(0)
{
    ui->setupUi(this);

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
    QFile file(":/json/json/skill.json");
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << Q_FUNC_INFO << "File open fail";
        return;
    }

    QJsonArray jArrData = QJsonDocument::fromJson(file.readAll()).array();
    file.close();

    for (const QJsonValue& jValData : jArrData)
    {
        const QJsonObject& jObjData = jValData.toObject();
        QString cls = jObjData.find("Class")->toString();
        const QJsonArray& jArrSkill = jObjData.find("Skills")->toArray();

        for (const QJsonValue& jValSkill : jArrSkill)
        {
            const QJsonObject& jObjSkill = jValSkill.toObject();
            const QString& skillName = jObjSkill.find("Text")->toString();
            m_classToSkills[cls].append({
                                            skillName,
                                            jObjSkill.find("Value")->toInt(),
                                            jObjSkill.find("IconPath")->toString()
                                        });
            const QJsonArray& jArrTripod = jObjSkill.find("Tripods")->toArray();

            for (const QJsonValue& jValTripod : jArrTripod)
            {
                const QJsonObject& jObjTripod = jValTripod.toObject();
                m_skillToTripods[skillName].append({
                                                       jObjTripod.find("Text")->toString(),
                                                       jObjTripod.find("Value")->toInt(),
                                                       jObjTripod.find("IconIndex")->toInt()
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
}

void TripodQuotation::setAlignments()
{
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
}

void TripodQuotation::clear()
{
    for (SkillInfoWidget* pSkillInfoWidget : m_skillInfoWidgets)
        delete pSkillInfoWidget;
    m_skillInfoWidgets.clear();
}

void TripodQuotation::addSkillInfoWidgets(QString cls)
{
    const QList<SkillInfo>& skillInfos = m_classToSkills[cls];
    for (const SkillInfo& skillInfo : skillInfos)
    {
        SkillInfoWidget* pSkillInfoWidget = new SkillInfoWidget(this, skillInfo, m_skillToTripods[skillInfo.name]);
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
