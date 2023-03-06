#include "skill_manager.h"
#include "resource/resource_manager.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

SkillManager *SkillManager::m_pInstance = nullptr;

SkillManager::SkillManager()
{
    initializeSkillList();
}

SkillManager::~SkillManager()
{
    destroyInstance();
}

void SkillManager::initializeSkillList()
{
    const QString skillIconPath = ":/image/skill/%1";
    const QString tripodIconPath = ":/image/tripod/Tripod_Tier_%1_%2.png";

    QJsonArray json = ResourceManager::getInstance()->loadJson("skill").find("Skill")->toArray();

    for (const QJsonValue& value : json)
    {
        const QJsonObject &object = value.toObject();

        // 직업별 스킬 목록 추가
        const QString &_class = object.find("Class")->toString();
        const QJsonArray skills = object.find("Skills")->toArray();

        for (const QJsonValue& value : skills)
        {
            const QJsonObject &skill = value.toObject();

            Skill newSkill;
            newSkill.setSkillName(skill.find("Text")->toString());
            newSkill.setSkillCode(skill.find("Value")->toInt());
            newSkill.setIconPath(skillIconPath.arg(skill.find("IconPath")->toString()));

            // 스킬별 트라이포드 목록 추가
            const QJsonArray &tripods = skill.find("Tripods")->toArray();

            for (int i = 0; i < tripods.size(); i++)
            {
                const QJsonObject &tripod = tripods[i].toObject();
                int tier = (i / 3) + 1;

                Tripod newTripod;
                newTripod.setTripodName(tripod.find("Text")->toString());
                newTripod.setTripodCode(tripod.find("Value")->toInt());
                if (newTripod.tripodCode() == -1)
                    newTripod.setMaxLevel(1);
                else
                    newTripod.setMaxLevel(5);
                newTripod.setTier(tier);
                newTripod.setIconPath(tripodIconPath.arg(tier).arg(tripod.find("IconIndex")->toInt()));

                newSkill.addTripod(newTripod);
            }
            m_skillList[_class].append(newSkill);
        }
    }
}

SkillManager *SkillManager::getInstance()
{
    if (m_pInstance == nullptr)
        m_pInstance = new SkillManager();

    return m_pInstance;
}

void SkillManager::destroyInstance()
{
    if (m_pInstance == nullptr)
        return;

    delete m_pInstance;
    m_pInstance = nullptr;
}

QList<Skill> SkillManager::skillList(const QString &_class) const
{
    return m_skillList[_class];
}
