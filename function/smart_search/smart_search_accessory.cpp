#include "smart_search_accessory.h"
#include "ui_smart_search_accessory.h"
#include "ui/widget_manager.h"
#include "game/engrave/engrave_manager.h"
#include "api/api_manager.h"

#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QGroupBox>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QFrame>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <algorithm>

const int MAX_SEARCH_COUNT = 3;

SmartSearchAccessory::SmartSearchAccessory(QLayout *pLayout) :
    ui(new Ui::SmartSearchAccessory),
    mSearchPage(0),
    mAddCount(0),
    mResponseCount(0),
    mSearchOptions(MAX_SEARCH_COUNT),
    mCurrentLayoutRows({0, 0, 0}),
    mpButtonSearchMore(nullptr),
    mpSearchButton(nullptr)
{
    ui->setupUi(this);

    pLayout->addWidget(this);
    this->hide();

    initializeOption();
    initializeOptionSelector();
    initializeOptionCode();
    initializeSearchButton();
    initializeResultUI();
    initializeSearchMore();
}

SmartSearchAccessory::~SmartSearchAccessory()
{
    delete mpSearchButton;
    delete mpButtonSearchMore;

    for (QWidget* pWidget : mOptionSelectors)
        delete pWidget;
    for (QWidget* pWidget : mWidgets)
        delete pWidget;
    for (QLayout* pLayout : mLayouts)
        delete pLayout;

    clearResult();

    delete ui;
}

void SmartSearchAccessory::refresh()
{
    if (mSearchResults.size() == 0)
    {
        mpSearchButton->setEnabled(true);
        return;
    }

    mResponseCount = 0;
    addSearchResult();
}

void SmartSearchAccessory::initializeOption()
{
    // 등급
    QStringList grades = {"유물", "고대"};
    mOptionItems << grades;

    // 부위
    QStringList parts = {"목걸이", "귀걸이", "반지"};
    mOptionItems << parts;

    // 특성
    QStringList abilities;
    for (int i = 0; i < static_cast<int>(Ability::size); i++)
        abilities << abilityToQString(static_cast<Ability>(i));
    for (int i = 0; i < 2; i++)
        mOptionItems << abilities;

    // 품질
    QStringList qualities;
    for (int i = 0; i < 10; i++)
        qualities << QString("%1 이상").arg(i * 10);
    mOptionItems << qualities;

    // 전투, 직업 각인
    QStringList engraves = {"선택 안함"};
    QStringList battleEngraves = EngraveManager::getInstance()->getBattleEngraves();
    QStringList classEngraves = EngraveManager::getInstance()->getClassEngraves();
    std::sort(battleEngraves.begin(), battleEngraves.end());
    std::sort(classEngraves.begin(), classEngraves.end());
    engraves << battleEngraves << classEngraves;
    for (int i = 0; i < 2; i++)
        mOptionItems << engraves;

    // 감소 각인
    QStringList penalties = {"선택 안함"};
    penalties << EngraveManager::getInstance()->getPenalties();
    mOptionItems << penalties;
}

void SmartSearchAccessory::initializeOptionSelector()
{
    const QStringList groupTitles = {"등급", "부위", "특성1", "특성2", "품질", "각인 (3고정)", "각인 (3~6)", "감소 각인"};

    for (int i = 0; i < groupTitles.size(); i++)
    {
        QGroupBox *pGroup = WidgetManager::createGroupBox(groupTitles[i]);
        ui->hLayoutOption->addWidget(pGroup);
        mWidgets.append(pGroup);

        QHBoxLayout *pLayout = new QHBoxLayout();
        pGroup->setLayout(pLayout);
        mLayouts.append(pLayout);

        QComboBox *pOptionSelector = WidgetManager::createComboBox(mOptionItems[i]);
        pLayout->addWidget(pOptionSelector);
        mOptionSelectors.append(pOptionSelector);
    }

    // 등급에 따라 각인 label 표기 변경
    // 유물 : 33, 34, 35 | 고대 : 34, 35, 36
    connect(mOptionSelectors[OptionIndex::Grade], &QComboBox::currentIndexChanged, this, [&](int index){
        const QString text = "각인(3%1)";

        for (int i = 0; i < mEngraveLabels.size(); i++)
            mEngraveLabels[i]->setText(text.arg(3 + i + index));
    });


    // 특성2는 목걸이를 선택한 경우만 활성화
    connect(mOptionSelectors[OptionIndex::Part], &QComboBox::currentIndexChanged, this, [&](int index){
        if (index != 0)
            mOptionSelectors[OptionIndex::Ability2]->setDisabled(true);
        else
            mOptionSelectors[OptionIndex::Ability2]->setEnabled(true);
    });
}

void SmartSearchAccessory::initializeOptionCode()
{
    mAbilityCodes["치명"] = static_cast<int>(AbilityCode::치명);
    mAbilityCodes["특화"] = static_cast<int>(AbilityCode::특화);
    mAbilityCodes["제압"] = static_cast<int>(AbilityCode::제압);
    mAbilityCodes["신속"] = static_cast<int>(AbilityCode::신속);
    mAbilityCodes["인내"] = static_cast<int>(AbilityCode::인내);
    mAbilityCodes["숙련"] = static_cast<int>(AbilityCode::숙련);

    mPartCodes["목걸이"] = CategoryCode::Necklace;
    mPartCodes["귀걸이"] = CategoryCode::Earring;
    mPartCodes["반지"] = CategoryCode::Ring;
}

void SmartSearchAccessory::initializeSearchButton()
{
    mpSearchButton = WidgetManager::createPushButton("검색", 10);
    ui->hLayoutOption->addWidget(mpSearchButton);

    // 검색 버튼 기능 구현
    connect(mpSearchButton, &QPushButton::released, this, [&](){
        mpSearchButton->setDisabled(true);

        // 각인이 선택되지 않은 경우 검색 중단
        for (int i = OptionIndex::Engrave1; i <= OptionIndex::Engrave2; i++)
        {
            QComboBox *pEngraveSelector = mOptionSelectors[i];
            if (pEngraveSelector->currentIndex() == 0)
            {
                QMessageBox msgBox;
                msgBox.setText("각인을 모두 선택해주세요.");
                msgBox.exec();
                mpSearchButton->setEnabled(true);
                return;
            }
        }

        mResponseCount = 0;
        mpButtonSearchMore->show();
        clearResult();

        setSearchOption();
        searchAccessory();
    });
}

void SmartSearchAccessory::initializeResultUI()
{
    mResultLayouts = {ui->gridResult1, ui->gridResult2, ui->gridResult3};

    for (QGridLayout* pLayout : mResultLayouts)
        pLayout->setAlignment(Qt::AlignTop);

    const QStringList attributes = {"#", "아이템명", "품질", "특성", "각인(3%1)", "즉시 구매가\n(최소 입찰가)"};
    for (int i = 0; i < mResultLayouts.size(); i++)
    {
        for (int j = 0; j < attributes.size(); j++)
        {
            QLabel *pLabelAttribute = nullptr;

            if (j == 4)
            {
                pLabelAttribute = WidgetManager::createLabel(attributes[j].arg(j - 1 + i), 12, "", 200, 50);
                mEngraveLabels.append(pLabelAttribute);
            }
            else
                pLabelAttribute = WidgetManager::createLabel(attributes[j], 12, "", 200, 50);

            mResultLayouts[i]->addWidget(pLabelAttribute, 0, j);
            mWidgets.append(pLabelAttribute);
        }
    }
}

void SmartSearchAccessory::initializeSearchMore()
{
    mpButtonSearchMore = WidgetManager::createPushButton("페이지 추가 검색", 10, 500, 30);
    mpButtonSearchMore->hide();
    ui->vLayoutMain->addWidget(mpButtonSearchMore);
    ui->vLayoutMain->setAlignment(mpButtonSearchMore, Qt::AlignHCenter);

    connect(mpButtonSearchMore, &QPushButton::released, this, &SmartSearchAccessory::searchAccessory);
}

void SmartSearchAccessory::setSearchOption()
{
    // 검색 옵션 세팅
    SearchOption baseSearchOption(SearchType::Auction);
    baseSearchOption.setItemTier(3);
    baseSearchOption.setSortCondition("ASC");

    // 각인2를 제외한 모든 옵션을 탐색하여 옵션 추가
    for (int i = 0; i < mOptionItems.size(); i++)
    {
        const QString &selectedOption = mOptionSelectors[i]->currentText();

        if (i == static_cast<int>(OptionIndex::Grade))
            baseSearchOption.setItemGrade(qStringToItemGrade(selectedOption));
        else if (i == static_cast<int>(OptionIndex::Part))
            baseSearchOption.setCategoryCode(mPartCodes[selectedOption]);
        else if (i == static_cast<int>(OptionIndex::Ability1))
            baseSearchOption.setEtcOption(EtcOptionCode::Ability, mAbilityCodes[selectedOption]);
        else if (i == static_cast<int>(OptionIndex::Ability2) && mOptionSelectors[OptionIndex::Ability2]->isEnabled())
            baseSearchOption.setEtcOption(EtcOptionCode::Ability, mAbilityCodes[selectedOption]);
        else if (i == static_cast<int>(OptionIndex::Quality))
            baseSearchOption.setQuality(selectedOption.chopped(3).toInt());
        else if (i == static_cast<int>(OptionIndex::Engrave1))
            baseSearchOption.setEtcOption(EtcOptionCode::Engrave, EngraveManager::getInstance()->getEngraveCode(selectedOption), 3, 3);
        else if (i == static_cast<int>(OptionIndex::Penalty) && mOptionSelectors[i]->currentIndex() != 0)
            baseSearchOption.setEtcOption(EtcOptionCode::Penalty, EngraveManager::getInstance()->getEngraveCode(selectedOption));
    }

    // 각인2의 가능한 모든 수치로 검색 옵션 생성
    for (int i = 0; i < MAX_SEARCH_COUNT; i++)
    {
        SearchOption *pSearchOption = new SearchOption(baseSearchOption);
        int value = mOptionSelectors[OptionIndex::Grade]->currentIndex() == 0 ? (i + 3) : (i + 4);

        const QString &engrave = mOptionSelectors[OptionIndex::Engrave2]->currentText();
        pSearchOption->setEtcOption(EtcOptionCode::Engrave, EngraveManager::getInstance()->getEngraveCode(engrave), value, value);

        mSearchOptions[i] = pSearchOption;
    }
}

void SmartSearchAccessory::searchAccessory()
{
    ++mSearchPage;

    for (int i = 0; i < MAX_SEARCH_COUNT; i++)
    {
        QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
        connect(pNetworkManager, &QNetworkAccessManager::finished, this, &SmartSearchAccessory::parseSearchResult);
        connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

        mSearchOptions[i]->setPageNo(mSearchPage);
        ApiManager::getInstance()->post(pNetworkManager, LostarkApi::Auction, QJsonDocument(mSearchOptions[i]->toJsonObject()).toJson());
    }
}

void SmartSearchAccessory::parseSearchResult(QNetworkReply *pReply)
{
    mResponseCount++;

    QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
    if (response.isNull())
        return;

    // 악세서리 정보 parsing
    const QJsonArray &items = response.object().find("Items")->toArray();
    for (const QJsonValue& value : items)
    {
        const QJsonObject &item = value.toObject();

        Accessory *pAccessory = new Accessory();
        pAccessory->setItemName(item.find("Name")->toString());
        pAccessory->setItemGrade(qStringToItemGrade(item.find("Grade")->toString()));
        pAccessory->setQuality(item.find("GradeQuality")->toInt());
        pAccessory->setIconPath(item.find("Icon")->toString());

        const QJsonArray &options = item.find("Options")->toArray();
        for (const QJsonValue& value : options)
        {
            const QJsonObject &option = value.toObject();

            const QString &type = option.find("Type")->toString();
            const QString &optionName = option.find("OptionName")->toString();
            const int &optionValue = option.find("Value")->toInt();

            if (type == "STAT")
                pAccessory->setAbility(qStringToAbility(optionName), optionValue);
            else if (type == "ABILITY_ENGRAVE")
            {
                if (option.find("IsPenalty")->toBool())
                    pAccessory->addPenalty(optionName, optionValue);
                else
                    pAccessory->addEngrave(optionName, optionValue);
            }
        }

        // 가격 정보 parsing
        const QJsonObject &auctionInfo = item.find("AuctionInfo")->toObject();
        int buyPrice = auctionInfo.find("BuyPrice")->toInt();
        int bidStartPrice = auctionInfo.find("BidStartPrice")->toInt();

        mSearchResults.append({pAccessory, {buyPrice, bidStartPrice}});
    }

    if (mResponseCount == MAX_SEARCH_COUNT)
        refresh();
}

void SmartSearchAccessory::addSearchResult()
{
    for (; mAddCount < mSearchResults.size(); mAddCount++)
    {
        const auto& result = mSearchResults[mAddCount];
        const Accessory *pAccessory = result.first;

        // 각인값의 합으로 layout index 설정
        int layoutIndex = 0;

        const QStringList &engraves = pAccessory->getEngraves();
        int sumOfEngraveValue = pAccessory->getEngraveValue(engraves[0]) + pAccessory->getEngraveValue(engraves[1]);

        if (pAccessory->itemGrade() == ItemGrade::유물)
            layoutIndex = sumOfEngraveValue - 6;
        else if (pAccessory->itemGrade() == ItemGrade::고대)
            layoutIndex = sumOfEngraveValue - 7;
        else
            continue;

        QGridLayout *pLayout = mResultLayouts[layoutIndex];
        int &row = mCurrentLayoutRows[layoutIndex];
        int col = 0;

        // 가로 구분선 추가
        pLayout->addWidget(createHLine(), ++row, 0, 1, -1);
        // 아이콘 추가
        pLayout->addWidget(createIcon(pAccessory->iconPath(), pAccessory->itemGrade()), ++row, col++);
        // 아이템명 추가
        pLayout->addWidget(createLabelItemName(pAccessory->itemName(), pAccessory->itemGrade()), row, col++);
        // 품질 추가
        pLayout->addWidget(createQualityBar(pAccessory->quality()), row, col++);
        // 특성 추가
        pLayout->addLayout(createAbilityLayout(pAccessory->abilities()), row, col++);
        // 증가, 감소 각인 추가
        pLayout->addLayout(createEngraveLayout(pAccessory), row, col++);
        // 가격 정보 추가
        pLayout->addWidget(createLabelPrice(result.second), row, col++);
    }

    // UI 업데이트 후 3초뒤 검색버튼 활성화
    QTimer::singleShot(3000, this, [&](){
        mpSearchButton->setEnabled(true);
    });
}

void SmartSearchAccessory::clearResult()
{
    mSearchPage = 0;
    mAddCount = 0;
    mCurrentLayoutRows = {0, 0, 0};

    for (SearchOption* pSearchOption : mSearchOptions)
    {
        delete pSearchOption;
        pSearchOption = nullptr;
    }

    for (int i = 0; i < mSearchResults.size(); i++)
        delete mSearchResults[i].first;
    mSearchResults.clear();

    for (QWidget* pWidget : mItemWidgets)
        delete pWidget;
    mItemWidgets.clear();

    for (QLayout* pLayout : mItemLayouts)
        delete pLayout;
    mItemLayouts.clear();
}

QFrame *SmartSearchAccessory::createHLine()
{
    QFrame *pHLine = WidgetManager::createLine(QFrame::HLine);
    mItemWidgets.append(pHLine);
    return pHLine;
}

QLabel *SmartSearchAccessory::createIcon(const QString &iconPath, const ItemGrade &itemGrade)
{
    QNetworkAccessManager *pIconLoader = new QNetworkAccessManager();
    connect(pIconLoader, &QNetworkAccessManager::finished, pIconLoader, &QNetworkAccessManager::deleteLater);

    QLabel *pIcon = WidgetManager::createIcon(iconPath, pIconLoader, itemGradeToBGColor(itemGrade));
    mItemWidgets.append(pIcon);

    return pIcon;
}

QLabel *SmartSearchAccessory::createLabelItemName(const QString &itemName, const ItemGrade &itemGrade)
{
    QLabel *pLabelItemName = WidgetManager::createLabel(itemName, 10, itemGradeToTextColor(itemGrade));
    mItemWidgets.append(pLabelItemName);
    return pLabelItemName;
}

QProgressBar *SmartSearchAccessory::createQualityBar(const int &quality)
{
    QProgressBar *pQualityBar = WidgetManager::createQualityBar(quality, 50, 20);
    mItemWidgets.append(pQualityBar);
    return pQualityBar;
}

QVBoxLayout *SmartSearchAccessory::createAbilityLayout(const QHash<Ability, int> &abilities)
{
    QVBoxLayout *pAbilityLayout = new QVBoxLayout();
    mItemLayouts.append(pAbilityLayout);

    for (auto iter = abilities.begin(); iter != abilities.end(); iter++)
    {
        QLabel *pLabelAbility = WidgetManager::createLabel(QString("%1 +%2").arg(abilityToQString(iter.key())).arg(iter.value()));
        pAbilityLayout->addWidget(pLabelAbility);
        mItemWidgets.append(pLabelAbility);
    }

    return pAbilityLayout;
}

QVBoxLayout *SmartSearchAccessory::createEngraveLayout(const Accessory *pAccessory)
{
    QVBoxLayout *pEngraveLayout = new QVBoxLayout();
    mItemLayouts.append(pEngraveLayout);

    const QStringList &engraves = pAccessory->getEngraves();
    for (const QString& engrave : engraves)
    {
        QLabel *pLabelEngrave = WidgetManager::createLabel(QString("%1 +%2").arg(engrave).arg(pAccessory->getEngraveValue(engrave)));
        pEngraveLayout->addWidget(pLabelEngrave);
        mItemWidgets.append(pLabelEngrave);
    }

    const QStringList &penalties = pAccessory->getPenalties();
    for (const QString& penalty : penalties)
    {
        QLabel *pLabelEngrave = WidgetManager::createLabel(QString("%1 +%2").arg(penalty).arg(pAccessory->getPenaltyValue(penalty)), 10, "red");
        pEngraveLayout->addWidget(pLabelEngrave);
        mItemWidgets.append(pLabelEngrave);
    }

    return pEngraveLayout;
}

QLabel *SmartSearchAccessory::createLabelPrice(const Price &price)
{
    QLabel *pLabelPrice = nullptr;
    if (price.buyPrice != 0)
        pLabelPrice = WidgetManager::createLabel(QString("%L1\n(%L2)").arg(price.buyPrice).arg(price.bidStartPrice), 10, "", 200, 50);
    else
        pLabelPrice = WidgetManager::createLabel(QString("%L1\n(%L2)").arg("-").arg(price.bidStartPrice), 10, "", 200, 50);
    mItemWidgets.append(pLabelPrice);
    return pLabelPrice;
}
