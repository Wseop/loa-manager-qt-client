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
    mIsWorking(false),
    mSearchResults(0),
    mTotalSearchCount(0),
    mCurrentSearchCount(0),
    mEngraveFilters(MAX_FILTER)
{
    ui->setupUi(this);
    ui->hLayoutMenu->setAlignment(Qt::AlignHCenter);

    pLayout->addWidget(this);
    this->hide();

    initializeValidEngraveList();
    initializeFilter();
    initializeResultUI();
}

SmartSearchAbilityStone::~SmartSearchAbilityStone()
{
    delete ui;
}

void SmartSearchAbilityStone::refresh()
{
    if (!mIsWorking) {
        searchAbilityStone();
    }
}

void SmartSearchAbilityStone::initializeValidEngraveList()
{
    mValidEngraves = {
        "정기 흡수", "안정된 상태", "원한", "슈퍼 차지", "구슬동자",
        "예리한 둔기", "급소 타격", "최대 마나 증가", "결투의 대가", "달인의 저력",
        "중갑 착용", "저주받은 인형", "기습의 대가", "바리케이드", "돌격대장",
        "각성", "질량 증가", "추진력", "타격의 대가", "아드레날린",
        "속전속결", "전문의", "정밀 단도"
    };

    std::sort(mValidEngraves.begin(), mValidEngraves.end());
}

void SmartSearchAbilityStone::initializeFilter()
{
    QGroupBox *pGroupEngraveFilter = WidgetManager::createGroupBox("각인 필터");
    QHBoxLayout *pEngraveFilterLayout = new QHBoxLayout();

    ui->hLayoutMenu->addWidget(pGroupEngraveFilter);
    pGroupEngraveFilter->setLayout(pEngraveFilterLayout);

    QStringList filterList;
    filterList << "각인 선택" << mValidEngraves;

    for (int i = 0; i < MAX_FILTER; i++) {
        QComboBox *pEngraveSelector = WidgetManager::createComboBox(filterList);
        pEngraveFilterLayout->addWidget(pEngraveSelector);
        mEngraveSelectors << pEngraveSelector;

        connect(pEngraveSelector, &QComboBox::currentIndexChanged, this, [&, i, pEngraveSelector](int index){
            if (index == 0) {
                mEngraveFilters[i] = "";
            } else {
                mEngraveFilters[i] = pEngraveSelector->currentText();
            }

            applyFilter();
        });
    }
}

void SmartSearchAbilityStone::initializeResultUI()
{
    const QStringList attributes = {"#", "아이템명", "각인1", "각인2", "즉시 구매가"};

    for (int col = 0; col < attributes.size(); col++)
    {
        QLabel *pLabelAttribute = WidgetManager::createLabel(attributes[col], 12, 200, 50);
        ui->gridResult->addWidget(pLabelAttribute, 0, col);
    }
}

void SmartSearchAbilityStone::searchAbilityStone()
{
    mIsWorking = true;
    clearResult();
    mTotalSearchCount = combination(mValidEngraves.size(), 2);

    // 가능한 모든 조합으로 검색
    for (int i = 0; i < mValidEngraves.size() - 1; i++)
    {
        for (int j = i + 1; j < mValidEngraves.size(); j++)
        {
            const QString &engrave1 = mValidEngraves[i];
            const QString &engrave2 = mValidEngraves[j];

            QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
            connect(pNetworkManager, &QNetworkAccessManager::finished,
                    this, &SmartSearchAbilityStone::parseSearchResult);
            connect(pNetworkManager, &QNetworkAccessManager::finished,
                    pNetworkManager, &QNetworkAccessManager::deleteLater);

            SearchOption searchOption(SearchType::Auction);
            searchOption.setCategoryCode(CategoryCode::AbilityStone);
            searchOption.setItemGrade(ItemGrade::유물);
            searchOption.setItemTier(3);
            searchOption.setPageNo(1);
            searchOption.setSortCondition("ASC");
            searchOption.setEtcOption(EtcOptionCode::Engrave, EngraveManager::getInstance()->getEngraveCode(engrave1));
            searchOption.setEtcOption(EtcOptionCode::Engrave, EngraveManager::getInstance()->getEngraveCode(engrave2));

            ApiManager::getInstance()->post(pNetworkManager,
                                            ApiType::Lostark,
                                            static_cast<int>(LostarkApi::Auction),
                                            {},
                                            QJsonDocument(searchOption.toJsonObject()).toJson());
        }
    }
}

void SmartSearchAbilityStone::parseSearchResult(QNetworkReply *pReply)
{
    mCurrentSearchCount++;

    QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
    if (!response.isNull()) {
        // 최저가 아이템 추출
        const auto &items = ResponseParser::parseAuctionItem(response).items;

        if (items.size() != 0) {
            const AuctionItem item = items.front();

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
            mSearchResults.append({abilityStone, item.auctionInfo});
        }
    }

    if (mTotalSearchCount == mCurrentSearchCount)
        showSearchResult();
}

void SmartSearchAbilityStone::showSearchResult()
{
    if (mSearchResults.size() == 0)
    {
        mIsWorking = false;
        return;
    }

    // 검색 결과 가격 내림차순 정렬
    std::sort(mSearchResults.begin(), mSearchResults.end(), [](const auto& a, const auto& b){
        return a.second.buyPrice > b.second.buyPrice;
    });

    // 검색 결과 출력
    int row = 0;

    for (int i = 0; i < mSearchResults.size(); i++) {
        const AbilityStone &abilityStone = mSearchResults[i].first;
        const AuctionInfo &auctionInfo = mSearchResults[i].second;

        QList<QWidget*> widgets;

        QFrame *pHLine = WidgetManager::createLine(QFrame::HLine);
        ui->gridResult->addWidget(pHLine, ++row, 0, 1, -1);
        widgets << pHLine;

        int col = 0;

        QLabel *pIcon = WidgetManager::createIcon(abilityStone.iconPath(),
                                                  nullptr,
                                                  itemGradeToBGColor(abilityStone.itemGrade()));
        ui->gridResult->addWidget(pIcon, ++row, col++);
        widgets << pIcon;

        QLabel *pLabelName = WidgetManager::createLabel(abilityStone.itemName());
        pLabelName->setStyleSheet(QString("QLabel { color: %1 }").arg(itemGradeToTextColor(abilityStone.itemGrade())));
        ui->gridResult->addWidget(pLabelName, row, col++);
        widgets << pLabelName;

        const QStringList &engraves = abilityStone.getEngrave()->getEngraves();

        for (int j = 0; j < engraves.size(); j++) {
            QLabel *pLabelEngrave = WidgetManager::createLabel(engraves[j]);
            ui->gridResult->addWidget(pLabelEngrave, row, col++);
            widgets << pLabelEngrave;

            mResultIndexMap[engraves[j]] << i;
        }

        QLabel *pLabelBuyPrice = WidgetManager::createLabel(QString("%L1").arg(auctionInfo.buyPrice), 10, 200, 50);
        ui->gridResult->addWidget(pLabelBuyPrice, row++, col);
        widgets << pLabelBuyPrice;

        mResultWidgets << widgets;
    }

    mIsWorking = false;
}

void SmartSearchAbilityStone::clearResult()
{
    mTotalSearchCount = 0;
    mCurrentSearchCount = 0;

    for (int i = 0; i < mResultWidgets.size(); i++) {
        for (int j = 0; j < mResultWidgets[i].size(); j++) {
            delete mResultWidgets[i][j];
        }
    }

    mSearchResults.clear();
    mResultWidgets.clear();
    mResultIndexMap.clear();
}

void SmartSearchAbilityStone::applyFilter()
{
    bool bApplyFilter = false;

    for (QString &filter : mEngraveFilters) {
        if (filter != "") {
            bApplyFilter = true;
            break;
        }
    }

    // 1개라도 선택된 경우 필터 적용
    if (bApplyFilter) {
        for (int i = 0; i < mResultWidgets.size(); i++) {
            for (int j = 0; j < mResultWidgets[i].size(); j++) {
                mResultWidgets[i][j]->hide();
            }
        }

        for (QString &filter : mEngraveFilters) {
            if (filter == "")
                continue;

            const QList<int> resultIndices = mResultIndexMap[filter];

            for (int i = 0; i < mResultWidgets.size(); i++) {
                QList<QWidget*> widgets = mResultWidgets[i];

                if (resultIndices.contains(i)) {
                    for (QWidget *pWidget : widgets)
                        pWidget->show();
                }
            }
        }
    } else {
        // 필터 미선택 시 전체 출력
        for (int i = 0; i < mResultWidgets.size(); i++) {
            for (int j = 0; j < mResultWidgets[i].size(); j++) {
                mResultWidgets[i][j]->show();
            }
        }
    }
}
