#include "skill_manager.h"
#include "api/api_manager.h"
#include "api/loamanager/loamanager_api.h"

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
    connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&, skillIconPath, tripodIconPath](QNetworkReply *pReply){
        QJsonArray data = QJsonDocument::fromJson(pReply->readAll()).array();

        for (const QJsonValue& value : data)
        {
            const QJsonObject &object = value.toObject();

            // 직업별 스킬 목록 추가
            const QString &_class = object.find("className")->toString();
            const QJsonArray skills = object.find("skills")->toArray();

            for (const QJsonValue& value : skills)
            {
                const QJsonObject &skill = value.toObject();

                Skill newSkill;
                newSkill.setSkillName(skill.find("skillName")->toString());
                newSkill.setSkillCode(skill.find("skillCode")->toInt());
                newSkill.setIconPath(skillIconPath.arg(skill.find("iconPath")->toString()));
                newSkill.setCounter(skill.find("isCounter")->toBool());

                // 스킬별 트라이포드 목록 추가
                const QJsonArray &tripods = skill.find("tripods")->toArray();

                for (int i = 0; i < tripods.size(); i++)
                {
                    const QJsonObject &tripod = tripods[i].toObject();
                    int tier = (i / 3) + 1;

                    Tripod newTripod;
                    newTripod.setTripodName(tripod.find("tripodName")->toString());
                    newTripod.setTripodCode(tripod.find("tripodCode")->toInt());
                    if (newTripod.tripodCode() == -1)
                        newTripod.setMaxLevel(1);
                    else
                        newTripod.setMaxLevel(5);
                    newTripod.setTier(tier);
                    newTripod.setIconPath(tripodIconPath.arg(tier).arg(tripod.find("iconIndex")->toInt()));

                    newSkill.addTripod(newTripod);
                }
                mSkillNames[_class] << newSkill.skillName();
                mSkills[_class][newSkill.skillName()] = newSkill;
            }
        }

        qDebug() << "SkillManager initialized";
    });
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);
    connect(pNetworkManager, &QNetworkAccessManager::finished, &mEventLoop, &QEventLoop::quit);

    ApiManager::getInstance()->get(pNetworkManager,
                                   ApiType::LoaManager,
                                   static_cast<int>(LoamanagerApi::GetResource),
                                   {"skill", ""},
                                   "");
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

QStringList SkillManager::skillNames(const QString &_class) const
{
    if (mSkillNames.contains(_class))
        return mSkillNames[_class];
    else
        return {};
}

QHash<QString, Skill> SkillManager::skills(const QString &_class) const
{
    if (mSkills.contains(_class))
        return mSkills[_class];
    else
        return {};
}
