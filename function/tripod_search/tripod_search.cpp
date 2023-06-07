#include "tripod_search.h"
#include "ui_tripod_search.h"
#include "ui/widget_manager.h"
#include "api/api_manager.h"
#include "api/lostark/search_option.h"
#include "game/skill/skill_manager.h"
#include "function/tripod_search/tripod_search_item.h"

#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <algorithm>

TripodSearch *TripodSearch::pInstance = nullptr;

TripodSearch::TripodSearch() :
    ui(new Ui::TripodSearch),
    mSearchHandleCount(0),
    pClassSelector(nullptr),
    pSearchButton(nullptr)
{
    ui->setupUi(this);
    ui->vLayoutItem->setAlignment(Qt::AlignTop);

    initializeInputLayout();
}

TripodSearch::~TripodSearch()
{
    delete ui;
}

void TripodSearch::initializeInputLayout()
{
    ui->hLayoutInput->setAlignment(Qt::AlignHCenter);
    ui->hLayoutInput->addWidget(createClassSelectorGroup());
    ui->hLayoutInput->addWidget(createSearchButton());
}

QGroupBox *TripodSearch::createClassSelectorGroup()
{
    QGroupBox *pClassSelectorGroup = WidgetManager::createGroupBox("직업 선택");

    QVBoxLayout *pClassSelectorGroupLayout = new QVBoxLayout();
    pClassSelectorGroupLayout->addWidget(createClassSelector());

    pClassSelectorGroup->setLayout(pClassSelectorGroupLayout);

    return pClassSelectorGroup;
}

QComboBox *TripodSearch::createClassSelector()
{
    pClassSelector = WidgetManager::createComboBox({});
    connect(pClassSelector, &QComboBox::currentTextChanged, this,
            [&](const QString &className)
    {
        mSelectedClass = className;
    });

    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished,
            pNetworkManager, &QNetworkAccessManager::deleteLater);
    connect(pNetworkManager, &QNetworkAccessManager::finished, this,
            [&](QNetworkReply *pReply)
    {
        if (!ApiManager::getInstance()->handleStatusCode(pReply))
            return;

        QJsonArray response = QJsonDocument::fromJson(pReply->readAll()).array();
        QStringList classNames;

        for (auto iter = response.constBegin(); iter != response.constEnd(); iter++)
            classNames << iter->toObject().find("child")->toVariant().toStringList();

        pClassSelector->addItems(classNames);
    });

    ApiManager::getInstance()->getResources(pNetworkManager, Resources::Class, "");

    return pClassSelector;
}

QPushButton *TripodSearch::createSearchButton()
{
    pSearchButton = WidgetManager::createPushButton("검색");

    connect(pSearchButton, &QPushButton::released, this, [&]()
    {
        pClassSelector->setDisabled(true);
        pSearchButton->setDisabled(true);

        searchTripod(getTripods());
    });

    return pSearchButton;
}

QList<QPair<QPair<QString, int>, QPair<QString, int>>> TripodSearch::getTripods()
{
    const SkillManager *pSkillManager = SkillManager::getInstance();
    const QHash<QString, Skill> &skills = pSkillManager->skills(mSelectedClass);

    QList<QPair<QPair<QString, int>, QPair<QString, int>>> result;

    for (auto iter = skills.constBegin(); iter != skills.constEnd(); iter++)
    {
        const QList<Tripod> &tripods = iter.value().tripods;
        QPair<QString, int> skillNameCode = {
            iter.value().skillName, iter.value().skillCode
        };

        for (const Tripod &tripod : tripods)
        {
            if (tripod.tripodCode == -1)
                continue;

            QPair<QString, int> tripodNameCode = {
                tripod.tripodName, tripod.tripodCode
            };

            result.append({skillNameCode, tripodNameCode});
        }
    }

    return result;
}

void TripodSearch::searchTripod(const QList<QPair<QPair<QString, int>, QPair<QString, int>>> &tripodCodes)
{
    int searchCount = tripodCodes.size();

    mSearchHandleCount = 0;
    mSearchResults.clear();

    for (const auto &tripodCode : tripodCodes)
    {
        AuctionSearchOption searchOption(AuctionCategory::아뮬렛);
        searchOption.setItemGrade(ItemGrade::유물);
        searchOption.addSkillCode(tripodCode.first.second);
        searchOption.addTripodCode(tripodCode.second.second);

        QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
        connect(pNetworkManager, &QNetworkAccessManager::finished,
                pNetworkManager, &QNetworkAccessManager::deleteLater);
        connect(pNetworkManager, &QNetworkAccessManager::finished, this,
                [&, tripodCode, searchCount](QNetworkReply *pReply)
        {
            if (!ApiManager::getInstance()->handleStatusCode(pReply))
            {
                if (++mSearchHandleCount == searchCount)
                    showSearchResult();

                return;
            }

            QJsonArray response = QJsonDocument::fromJson(pReply->readAll()).array();
            if (response.empty())
            {
                if (++mSearchHandleCount == searchCount)
                    showSearchResult();

                return;
            }

            const QJsonObject &item = response[0].toObject();

            AuctionItem auctionItem;
            auctionItem.itemName = item.find("itemName")->toString();
            auctionItem.itemGrade = item.find("itemGrade")->toString();
            auctionItem.iconPath = item.find("iconPath")->toString();

            const QJsonArray itemOptions = item.find("itemOptions")->toArray();
            for (auto iter = itemOptions.constBegin(); iter != itemOptions.constEnd(); iter++)
            {
                const QString &skillName = iter->toObject().find("optionName")
                                               ->toString();
                const QString &tripodName = iter->toObject().find("tripod")
                                                ->toString();

                if (skillName == tripodCode.first.first &&
                    tripodName == tripodCode.second.first)
                {
                    AuctionItemOption itemOption;
                    itemOption.optionName = skillName;
                    itemOption.tripod = tripodName;
                    itemOption.value = iter->toObject().find("value")->toInt();

                    auctionItem.itemOptions << itemOption;
                    break;
                }
            }

            auctionItem.buyPrice = item.contains("buyPrice")
                                       ? item["buyPrice"].toInt()
                                       : -1;
            auctionItem.startPrice = item.contains("startPrice")
                                         ? item["startPrice"].toInt()
                                         : -1;
            auctionItem.currentBidPrice = item.contains("currentBidPrice")
                                              ? item["currentBidPrice"].toInt()
                                              : -1;
            auctionItem.nextBidPrice = item.contains("nextBidPrice")
                                           ? item["nextBidPrice"].toInt()
                                           : -1;

            mSearchResults << auctionItem;

            if (++mSearchHandleCount == searchCount)
            {
                showSearchResult();
                pClassSelector->setEnabled(true);
                pSearchButton->setEnabled(true);
            }
        });

        ApiManager::getInstance()->getAuctionItems(
            pNetworkManager, searchOption.getQuery());
    }
}

void TripodSearch::showSearchResult()
{
    for (TripodSearchItem *pItem : mTripodSearchItems)
        delete pItem;
    mTripodSearchItems.clear();

    std::sort(mSearchResults.begin(), mSearchResults.end(), [&](auto a, auto b){
        return a.buyPrice > b.buyPrice;
    });

    for (const AuctionItem &item : mSearchResults)
    {
        TripodSearchItem *pTripodSearchItem = new TripodSearchItem(item,
                                                                   mSelectedClass);
        ui->vLayoutItem->addWidget(pTripodSearchItem);
        mTripodSearchItems << pTripodSearchItem;
    }
}

void TripodSearch::refresh()
{

}

TripodSearch *TripodSearch::getInstance()
{
    if (pInstance == nullptr)
        pInstance = new TripodSearch();

    return pInstance;
}

void TripodSearch::destroyInstance()
{
    if (pInstance == nullptr)
        return;

    delete pInstance;
    pInstance = nullptr;
}
