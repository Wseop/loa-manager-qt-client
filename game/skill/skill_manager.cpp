#include "skill_manager.h"
#include "api/api_manager.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>

SkillManager *SkillManager::mpInstance = nullptr;

SkillManager::SkillManager()
{
    initializeSkillList();
}

SkillManager::~SkillManager()
{

}

void SkillManager::initializeSkillList()
{
    const QString skillIconPath = ":/image/skill/%1";
    const QString tripodIconPath = ":/image/tripod/Tripod_Tier_%1_%2.png";

    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();

    connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&, skillIconPath, tripodIconPath](QNetworkReply *pReply)
    {
        QJsonArray data = QJsonDocument::fromJson(pReply->readAll()).array();

        for (const QJsonValue& value : data)
        {
            const QJsonObject &object = value.toObject();

            // 직업별 스킬 목록 추가
            const QString &className = object.find("className")->toString();
            const QJsonArray &skills = object.find("skills")->toArray();

            for (const QJsonValue& value : skills)
            {
                const QJsonObject &skill = value.toObject();

                Skill newSkill;
                newSkill.skillName = skill.find("skillName")->toString();
                newSkill.skillCode = skill.find("skillCode")->toInt();
                newSkill.iconPath = skillIconPath.arg(skill.find("iconPath")->toString());
                newSkill.isCounter = skill.find("isCounter")->toBool();

                // 스킬별 트라이포드 목록 추가
                const QJsonArray &tripods = skill.find("tripods")->toArray();

                for (int i = 0; i < tripods.size(); i++)
                {
                    const QJsonObject &tripod = tripods[i].toObject();
                    int tier = (i / 3) + 1;

                    Tripod newTripod;
                    newTripod.tripodName = tripod.find("tripodName")->toString();
                    newTripod.tripodCode = tripod.find("tripodCode")->toInt();

                    if (newTripod.tripodCode == -1)
                        newTripod.maxLevel = 1;
                    else
                        newTripod.maxLevel = 5;

                    newTripod.tier = tier;
                    newTripod.iconPath = tripodIconPath.arg(tier).arg(tripod.find("iconIndex")->toInt());

                    newSkill.tripods << newTripod;
                }

                mSkillMap[className][newSkill.skillName] = newSkill;
            }
        }

        qDebug() << "SkillManager initialized";
    });
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);
    connect(pNetworkManager, &QNetworkAccessManager::finished, &mEventLoop, &QEventLoop::quit);

    ApiManager::getInstance()->getResources(pNetworkManager, Resources::Skill, "");

    mEventLoop.exec();
}

SkillManager *SkillManager::getInstance()
{
    if (mpInstance == nullptr)
        mpInstance = new SkillManager();

    return mpInstance;
}

void SkillManager::destroyInstance()
{
    if (mpInstance == nullptr)
        return;

    delete mpInstance;
    mpInstance = nullptr;
}

QStringList SkillManager::skillNames(const QString &className) const
{
    if (mSkillMap.contains(className)) {
        return mSkillMap[className].keys();
    } else {
        return {};
    }
}

QHash<QString, Skill> SkillManager::skills(const QString &className) const
{
    if (mSkillMap.contains(className)) {
        return mSkillMap[className];
    } else {
        return QHash<QString, Skill> {};
    }
}
