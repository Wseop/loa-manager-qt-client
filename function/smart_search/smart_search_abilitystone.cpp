#include "smart_search_abilitystone.h"
#include "ui_smart_search_abilitystone.h"
#include "ui/widget_manager.h"
#include "api/api_manager.h"
#include "api/response_parser.h"
#include "api/lostark/search_option.h"
#include "game/engrave/engrave_manager.h"
#include "util/util.h"

#include <algorithm>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>

SmartSearchAbilityStone::SmartSearchAbilityStone(QLayout *pLayout) :
    ui(new Ui::SmartSearchAbilityStone),
    mSearchResults(0),
    mTotalSearchCount(0),
    mCurrentSearchCount(0)
{
    ui->setupUi(this);
    ui->hLayoutMenu->setAlignment(Qt::AlignHCenter);

    pLayout->addWidget(this);
    this->hide();

    initializeEngraveSelector();
    initializeResultUI();
}

SmartSearchAbilityStone::~SmartSearchAbilityStone()
{
    clearResult();

    for (QComboBox* pEngraveSelector : mEngraveSelectors)
        delete pEngraveSelector;
    delete mpPenaltySelector;

    for (QWidget* pWidget : mWidgets)
        delete pWidget;

    for (auto rIter = mLayouts.rbegin(); rIter != mLayouts.rend(); rIter++)
        delete *rIter;
    mLayouts.clear();

    delete ui;
}

void SmartSearchAbilityStone::refresh()
{
    if (mSearchResults.size() == 0)
        return;

    // 검색 결과 최저가 순으로 정렬
    std::sort(mSearchResults.begin(), mSearchResults.end(), [](const auto& a, const auto& b){
        return a.second.buyPrice < b.second.buyPrice;
    });

    // 검색 결과 출력
    int row = 0;
    for (const auto& searchResult : mSearchResults)
    {
        const AbilityStone &abilityStone = searchResult.first;
        const Price &price = searchResult.second;

        QFrame *pHLine = WidgetManager::createLine(QFrame::HLine);
        ui->gridResult->addWidget(pHLine, ++row, 0, 1, -1);
        mResultWidgets.append(pHLine);

        QLabel *pIcon = WidgetManager::createIcon(abilityStone.iconPath(), nullptr, itemGradeToBGColor(abilityStone.itemGrade()));
        ui->gridResult->addWidget(pIcon, ++row, 0);
        mResultWidgets.append(pIcon);

        QLabel *pLabelName = WidgetManager::createLabel(abilityStone.itemName(), 10, itemGradeToTextColor(abilityStone.itemGrade()));
        ui->gridResult->addWidget(pLabelName, row, 1);
        mResultWidgets.append(pLabelName);

        const QStringList &engraves = abilityStone.getEngrave()->getEngraves();
        for (int i = 0; i < engraves.size(); i++)
        {
            QLabel *pLabelEngrave = WidgetManager::createLabel(engraves[i]);
            ui->gridResult->addWidget(pLabelEngrave, row, 2 + i);
            mResultWidgets.append(pLabelEngrave);
        }

        QLabel *pLabelPenalty = WidgetManager::createLabel(abilityStone.getEngrave()->getPenalties().at(0), 10, "red");
        ui->gridResult->addWidget(pLabelPenalty, row, 4);
        mResultWidgets.append(pLabelPenalty);

        QLabel *pLabelPrice = WidgetManager::createLabel(QString("%L1\n(%L2)").arg(price.buyPrice).arg(price.bidStartPrice), 10, "", 200, 50);
        ui->gridResult->addWidget(pLabelPrice, row++, 5);
        mResultWidgets.append(pLabelPrice);
    }
}

void SmartSearchAbilityStone::initializeEngraveSelector()
{
    const int MAX_SELECTOR = 5;

    // 전투 각인 오름차순 정렬
    QStringList battleEngraves = EngraveManager::getInstance()->getBattleEngraves();
    std::sort(battleEngraves.begin(), battleEngraves.end());

    // 각인 선택기 추가
    QStringList engraves = {"선택 안함"};
    engraves << battleEngraves;

    QGroupBox *pGroupEngrave = WidgetManager::createGroupBox("각인 선택");
    ui->hLayoutMenu->addWidget(pGroupEngrave);
    mWidgets.append(pGroupEngrave);

    QHBoxLayout *pLayoutGroupEngrave = new QHBoxLayout();
    pGroupEngrave->setLayout(pLayoutGroupEngrave);
    mLayouts.append(pLayoutGroupEngrave);

    for (int i = 0; i < MAX_SELECTOR; i++)
    {
        QComboBox *pEngraveSelector = WidgetManager::createComboBox(engraves);
        pLayoutGroupEngrave->addWidget(pEngraveSelector);
        mEngraveSelectors.append(pEngraveSelector);
    }

    // 감소 각인 선택기 추가
    QStringList penalties = {"선택 안함"};
    penalties << EngraveManager::getInstance()->getPenalties();

    QGroupBox *pGroupPenalty = WidgetManager::createGroupBox("감소 각인 선택");
    ui->hLayoutMenu->addWidget(pGroupPenalty);
    mWidgets.append(pGroupPenalty);

    QHBoxLayout *pLayoutGroupPenalty = new QHBoxLayout();
    pGroupPenalty->setLayout(pLayoutGroupPenalty);
    mLayouts.append(pLayoutGroupPenalty);

    mpPenaltySelector = WidgetManager::createComboBox(penalties);
    pLayoutGroupPenalty->addWidget(mpPenaltySelector);

    // 검색 버튼 추가
    QPushButton *pButtonSearch = WidgetManager::createPushButton("검색");
    connect(pButtonSearch, &QPushButton::released, this, &SmartSearchAbilityStone::searchAbilityStone);
    ui->hLayoutMenu->addWidget(pButtonSearch);
    mWidgets.append(pButtonSearch);
}

void SmartSearchAbilityStone::initializeResultUI()
{
    const QStringList attributes = {"#", "아이템명", "각인1", "각인2", "감소", "즉시 구매가\n(최소 입찰가)"};

    for (int col = 0; col < attributes.size(); col++)
    {
        QLabel *pLabelAttribute = WidgetManager::createLabel(attributes[col], 12, "", 200, 50);
        ui->gridResult->addWidget(pLabelAttribute, 0, col);
        mWidgets.append(pLabelAttribute);
    }
}

void SmartSearchAbilityStone::searchAbilityStone()
{
    // 선택한 각인 취합 (중복X)
    QSet<QString> engraves;
    for (const QComboBox* pEngraveSelector : mEngraveSelectors)
    {
        if (pEngraveSelector->currentIndex() != 0)
            engraves.insert(pEngraveSelector->currentText());
    }

    // 선택된 각인이 2개 미만이면 검색 취소
    if (engraves.size() < 2)
    {
        QMessageBox msgBox;
        msgBox.setText("각인을 2개 이상 선택해주세요.");
        msgBox.exec();
        return;
    }

    clearResult();

    // 가능한 모든 조합으로 검색
    const QStringList selectedEngraves = engraves.values();
    mTotalSearchCount = combination(selectedEngraves.size(), 2);

    for (int i = 0; i < selectedEngraves.size() - 1; i++)
    {
        for (int j = i + 1; j < selectedEngraves.size(); j++)
        {
            const QString &engrave1 = selectedEngraves[i];
            const QString &engrave2 = selectedEngraves[j];

            QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
            connect(pNetworkManager, &QNetworkAccessManager::finished, this, &SmartSearchAbilityStone::parseSearchResult);
            connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

            // 어빌리티 스톤 검색 옵션 세팅 후 api 요청 전달
            SearchOption searchOption(SearchType::Auction);
            searchOption.setCategoryCode(CategoryCode::AbilityStone);
            searchOption.setItemGrade(ItemGrade::유물);
            searchOption.setItemTier(3);
            searchOption.setPageNo(1);
            searchOption.setSortCondition("ASC");
            searchOption.setEtcOption(EtcOptionCode::Engrave, EngraveManager::getInstance()->getEngraveCode(engrave1));
            searchOption.setEtcOption(EtcOptionCode::Engrave, EngraveManager::getInstance()->getEngraveCode(engrave2));
            if (mpPenaltySelector->currentIndex() != 0)
                searchOption.setEtcOption(EtcOptionCode::Engrave, EngraveManager::getInstance()->getEngraveCode(mpPenaltySelector->currentText()));

            ApiManager::getInstance()->post(pNetworkManager, ApiType::Lostark, static_cast<int>(LostarkApi::Auction), QJsonDocument(searchOption.toJsonObject()).toJson());
        }
    }
}

void SmartSearchAbilityStone::parseSearchResult(QNetworkReply *pReply)
{
    mCurrentSearchCount++;

    QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
    if (response.isNull())
        return;

    ResponseAuction responseAuction = ResponseParser::parseAuctionItem(response);
    const AuctionItem &item = responseAuction.items.front();

    // 검색 결과 parsing (최저가 1개)
    int buyPrice = item.AuctionInfo.buyPrice;
    int bidStartPrice = item.AuctionInfo.bidStartPrice;

    AbilityStone abilityStone;
    abilityStone.setItemName(item.name);
    abilityStone.setItemGrade(qStringToItemGrade(item.grade));
    abilityStone.setIconPath(":/image/item/abilitystone/0.png");

    const QList<AuctionItemOption> &options = item.options;

    for (const AuctionItemOption &option : options)
    {
        const QString &engrave = option.optionName;

        if (option.bPenalty)
            abilityStone.getEngrave()->addPenalty(engrave, 0);
        else
            abilityStone.getEngrave()->addEngrave(engrave, 0);
    }

    // 검색 결과 추가
    mSearchResults.append({abilityStone, {buyPrice, bidStartPrice}});

    // 검색 완료 시 UI 갱신
    if (mTotalSearchCount == mCurrentSearchCount)
        refresh();
}

void SmartSearchAbilityStone::clearResult()
{
    mSearchResults.clear();

    mTotalSearchCount = 0;
    mCurrentSearchCount = 0;

    for (QWidget* pWidget : mResultWidgets)
        delete pWidget;
    mResultWidgets.clear();
}
