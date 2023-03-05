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

SmartSearchAccessory::SmartSearchAccessory(QLayout* pLayout) :
    ui(new Ui::SmartSearchAccessory),
    m_pSearchButton(nullptr),
    m_searchPage(0),
    m_addCount(0),
    m_responseCount(0),
    m_searchOptions(MAX_SEARCH_COUNT),
    m_currentLayoutRows({0, 0, 0}),
    m_pButtonSearchMore(nullptr)
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
    delete m_pSearchButton;
    delete m_pButtonSearchMore;

    for (QWidget* pWidget : m_optionSelectors)
        delete pWidget;
    for (QWidget* pWidget : m_widgets)
        delete pWidget;
    for (QLayout* pLayout : m_layouts)
        delete pLayout;

    clearResult();

    delete ui;
}

void SmartSearchAccessory::refresh()
{
    if (m_searchResults.size() == 0)
    {
        m_pSearchButton->setEnabled(true);
        return;
    }

    m_responseCount = 0;
    addSearchResult();
}

void SmartSearchAccessory::initializeOption()
{
    // 등급
    QStringList grades = {"유물", "고대"};
    m_optionItems << grades;

    // 부위
    QStringList parts = {"목걸이", "귀걸이", "반지"};
    m_optionItems << parts;

    // 특성
    QStringList abilities;
    for (int i = 0; i < static_cast<int>(Ability::size); i++)
        abilities << abilityToQString(static_cast<Ability>(i));
    for (int i = 0; i < 2; i++)
        m_optionItems << abilities;

    // 품질
    QStringList qualities;
    for (int i = 0; i < 10; i++)
        qualities << QString("%1 이상").arg(i * 10);
    m_optionItems << qualities;

    // 전투, 직업 각인
    QStringList engraves = {"선택 안함"};
    QStringList battleEngraves = EngraveManager::getInstance()->getBattleEngraves();
    QStringList classEngraves = EngraveManager::getInstance()->getClassEngraves();
    std::sort(battleEngraves.begin(), battleEngraves.end());
    std::sort(classEngraves.begin(), classEngraves.end());
    engraves << battleEngraves << classEngraves;
    for (int i = 0; i < 2; i++)
        m_optionItems << engraves;

    // 감소 각인
    QStringList penalties = {"선택 안함"};
    penalties << EngraveManager::getInstance()->getPenalties();
    m_optionItems << penalties;
}

void SmartSearchAccessory::initializeOptionSelector()
{
    const QStringList groupTitles = {"등급", "부위", "특성1", "특성2", "품질", "각인 (3고정)", "각인 (3~6)", "감소 각인"};

    for (int i = 0; i < groupTitles.size(); i++)
    {
        QGroupBox* pGroup = WidgetManager::createGroupBox(groupTitles[i]);
        ui->hLayoutOption->addWidget(pGroup);
        m_widgets.append(pGroup);

        QHBoxLayout* pLayout = new QHBoxLayout();
        pGroup->setLayout(pLayout);
        m_layouts.append(pLayout);

        QComboBox* pOptionSelector = WidgetManager::createComboBox(m_optionItems[i]);
        pLayout->addWidget(pOptionSelector);
        m_optionSelectors.append(pOptionSelector);
    }

    // 등급에 따라 각인 label 표기 변경
    // 유물 : 33, 34, 35 | 고대 : 34, 35, 36
    connect(m_optionSelectors[OptionIndex::Grade], &QComboBox::currentIndexChanged, this, [&](int index){
        const QString text = "각인(3%1)";

        for (int i = 0; i < m_engraveLabels.size(); i++)
            m_engraveLabels[i]->setText(text.arg(3 + i + index));
    });


    // 특성2는 목걸이를 선택한 경우만 활성화
    connect(m_optionSelectors[OptionIndex::Part], &QComboBox::currentIndexChanged, this, [&](int index){
        if (index != 0)
            m_optionSelectors[OptionIndex::Ability2]->setDisabled(true);
        else
            m_optionSelectors[OptionIndex::Ability2]->setEnabled(true);
    });
}

void SmartSearchAccessory::initializeOptionCode()
{
    m_abilityCodes["치명"] = static_cast<int>(AbilityCode::치명);
    m_abilityCodes["특화"] = static_cast<int>(AbilityCode::특화);
    m_abilityCodes["제압"] = static_cast<int>(AbilityCode::제압);
    m_abilityCodes["신속"] = static_cast<int>(AbilityCode::신속);
    m_abilityCodes["인내"] = static_cast<int>(AbilityCode::인내);
    m_abilityCodes["숙련"] = static_cast<int>(AbilityCode::숙련);

    m_partCodes["목걸이"] = CategoryCode::Necklace;
    m_partCodes["귀걸이"] = CategoryCode::Earring;
    m_partCodes["반지"] = CategoryCode::Ring;
}

void SmartSearchAccessory::initializeSearchButton()
{
    m_pSearchButton = WidgetManager::createPushButton("검색", 10);
    ui->hLayoutOption->addWidget(m_pSearchButton);

    // 검색 버튼 기능 구현
    connect(m_pSearchButton, &QPushButton::released, this, [&](){
        m_pSearchButton->setDisabled(true);

        // 각인이 선택되지 않은 경우 검색 중단
        for (int i = OptionIndex::Engrave1; i <= OptionIndex::Engrave2; i++)
        {
            QComboBox* pEngraveSelector = m_optionSelectors[i];
            if (pEngraveSelector->currentIndex() == 0)
            {
                QMessageBox msgBox;
                msgBox.setText("각인을 모두 선택해주세요.");
                msgBox.exec();
                m_pSearchButton->setEnabled(true);
                return;
            }
        }

        m_responseCount = 0;
        m_pButtonSearchMore->show();
        clearResult();

        // 검색 옵션 세팅
        SearchOption baseSearchOption(SearchType::Auction);
        baseSearchOption.setItemTier(3);
        baseSearchOption.setSortCondition("ASC");

        // 각인2를 제외한 모든 옵션을 탐색하여 옵션 추가
        for (int i = 0; i < m_optionItems.size(); i++)
        {
            const QString& selectedOption = m_optionSelectors[i]->currentText();

            if (i == static_cast<int>(OptionIndex::Grade))
                baseSearchOption.setItemGrade(qStringToItemGrade(selectedOption));
            else if (i == static_cast<int>(OptionIndex::Part))
                baseSearchOption.setCategoryCode(m_partCodes[selectedOption]);
            else if (i == static_cast<int>(OptionIndex::Ability1))
                baseSearchOption.setEtcOption(EtcOptionCode::Ability, m_abilityCodes[selectedOption]);
            else if (i == static_cast<int>(OptionIndex::Ability2) && m_optionSelectors[OptionIndex::Ability2]->isEnabled())
                baseSearchOption.setEtcOption(EtcOptionCode::Ability, m_abilityCodes[selectedOption]);
            else if (i == static_cast<int>(OptionIndex::Quality))
                baseSearchOption.setQuality(selectedOption.chopped(3).toInt());
            else if (i == static_cast<int>(OptionIndex::Engrave1))
                baseSearchOption.setEtcOption(EtcOptionCode::Engrave, EngraveManager::getInstance()->getEngraveCode(selectedOption), 3, 3);
            else if (i == static_cast<int>(OptionIndex::Penalty) && m_optionSelectors[i]->currentIndex() != 0)
                baseSearchOption.setEtcOption(EtcOptionCode::Penalty, EngraveManager::getInstance()->getEngraveCode(selectedOption));
        }

        // 각인2의 가능한 모든 수치로 검색 옵션 생성
        for (int i = 0; i < MAX_SEARCH_COUNT; i++)
        {
            SearchOption* pSearchOption = new SearchOption(baseSearchOption);
            int value = m_optionSelectors[OptionIndex::Grade]->currentIndex() == 0 ? (i + 3) : (i + 4);

            const QString& engrave = m_optionSelectors[OptionIndex::Engrave2]->currentText();
            pSearchOption->setEtcOption(EtcOptionCode::Engrave, EngraveManager::getInstance()->getEngraveCode(engrave), value, value);

            m_searchOptions[i] = pSearchOption;
        }

        searchAccessory();
    });
}

void SmartSearchAccessory::initializeResultUI()
{
    m_resultLayouts = {ui->gridResult1, ui->gridResult2, ui->gridResult3};
    for (QGridLayout* pLayout : m_resultLayouts)
        pLayout->setAlignment(Qt::AlignTop);

    const QStringList attributes = {"#", "아이템명", "품질", "특성", "각인(3%1)", "즉시 구매가\n(최소 입찰가)"};
    for (int i = 0; i < m_resultLayouts.size(); i++)
    {
        for (int j = 0; j < attributes.size(); j++)
        {
            QLabel* pLabelAttribute = nullptr;

            if (j == 4)
            {
                pLabelAttribute = WidgetManager::createLabel(attributes[j].arg(j - 1 + i), 12, "", 200, 50);
                m_engraveLabels.append(pLabelAttribute);
            }
            else
                pLabelAttribute = WidgetManager::createLabel(attributes[j], 12, "", 200, 50);

            m_resultLayouts[i]->addWidget(pLabelAttribute, 0, j);
            m_widgets.append(pLabelAttribute);
        }
    }
}

void SmartSearchAccessory::initializeSearchMore()
{
    m_pButtonSearchMore = WidgetManager::createPushButton("페이지 추가 검색", 10, 500, 30);
    m_pButtonSearchMore->hide();
    ui->vLayoutMain->addWidget(m_pButtonSearchMore);
    ui->vLayoutMain->setAlignment(m_pButtonSearchMore, Qt::AlignHCenter);

    connect(m_pButtonSearchMore, &QPushButton::released, this, &SmartSearchAccessory::searchAccessory);
}

void SmartSearchAccessory::searchAccessory()
{
    ++m_searchPage;

    for (int i = 0; i < MAX_SEARCH_COUNT; i++)
    {
        QNetworkAccessManager* pNetworkManager = new QNetworkAccessManager();
        connect(pNetworkManager, &QNetworkAccessManager::finished, this, &SmartSearchAccessory::parseSearchResult);
        connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

        m_searchOptions[i]->setPageNo(m_searchPage);
        ApiManager::getInstance()->post(pNetworkManager, LostarkApi::Auction, QJsonDocument(m_searchOptions[i]->toJsonObject()).toJson());
    }
}

void SmartSearchAccessory::parseSearchResult(QNetworkReply* pReply)
{
    m_responseCount++;

    QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
    if (response.isNull())
        return;

    // 악세서리 정보 parsing
    const QJsonArray& items = response.object().find("Items")->toArray();
    for (const QJsonValue& value : items)
    {
        const QJsonObject& item = value.toObject();

        Accessory* pAccessory = new Accessory();
        pAccessory->setItemName(item.find("Name")->toString());
        pAccessory->setItemGrade(qStringToItemGrade(item.find("Grade")->toString()));
        pAccessory->setQuality(item.find("GradeQuality")->toInt());
        pAccessory->setIconPath(item.find("Icon")->toString());

        const QJsonArray& options = item.find("Options")->toArray();
        for (const QJsonValue& value : options)
        {
            const QJsonObject& option = value.toObject();

            const QString& type = option.find("Type")->toString();
            const QString& optionName = option.find("OptionName")->toString();
            const int& optionValue = option.find("Value")->toInt();

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
        const QJsonObject& auctionInfo = item.find("AuctionInfo")->toObject();
        int buyPrice = auctionInfo.find("BuyPrice")->toInt();
        int bidStartPrice = auctionInfo.find("BidStartPrice")->toInt();

        m_searchResults.append({pAccessory, {buyPrice, bidStartPrice}});
    }

    if (m_responseCount == MAX_SEARCH_COUNT)
        refresh();
}

void SmartSearchAccessory::addSearchResult()
{
    for (; m_addCount < m_searchResults.size(); m_addCount++)
    {
        const auto& result = m_searchResults[m_addCount];
        const Accessory* pAccessory = result.first;

        // 각인값의 합으로 layout index 설정
        int layoutIndex = 0;

        const QStringList& engraves = pAccessory->getEngraves();
        int sumOfEngraveValue = pAccessory->getEngraveValue(engraves[0]) + pAccessory->getEngraveValue(engraves[1]);

        if (pAccessory->itemGrade() == ItemGrade::유물)
            layoutIndex = sumOfEngraveValue - 6;
        else if (pAccessory->itemGrade() == ItemGrade::고대)
            layoutIndex = sumOfEngraveValue - 7;
        else
            continue;

        QGridLayout* pLayout = m_resultLayouts[layoutIndex];
        int& row = m_currentLayoutRows[layoutIndex];
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
        m_pSearchButton->setEnabled(true);
    });
}

void SmartSearchAccessory::clearResult()
{
    m_searchPage = 0;
    m_addCount = 0;
    m_currentLayoutRows = {0, 0, 0};

    for (SearchOption* pSearchOption : m_searchOptions)
    {
        delete pSearchOption;
        pSearchOption = nullptr;
    }

    for (int i = 0; i < m_searchResults.size(); i++)
        delete m_searchResults[i].first;
    m_searchResults.clear();

    for (QWidget* pWidget : m_itemWidgets)
        delete pWidget;
    m_itemWidgets.clear();

    for (QLayout* pLayout : m_itemLayouts)
        delete pLayout;
    m_itemLayouts.clear();
}

QFrame* SmartSearchAccessory::createHLine()
{
    QFrame* pHLine = WidgetManager::createLine(QFrame::HLine);
    m_itemWidgets.append(pHLine);
    return pHLine;
}

QLabel* SmartSearchAccessory::createIcon(const QString& iconPath, const ItemGrade& itemGrade)
{
    QNetworkAccessManager* pIconLoader = new QNetworkAccessManager();
    connect(pIconLoader, &QNetworkAccessManager::finished, pIconLoader, &QNetworkAccessManager::deleteLater);

    QLabel* pIcon = WidgetManager::createIcon(iconPath, pIconLoader, itemGradeToBGColor(itemGrade));
    m_itemWidgets.append(pIcon);

    return pIcon;
}

QLabel* SmartSearchAccessory::createLabelItemName(const QString& itemName, const ItemGrade& itemGrade)
{
    QLabel* pLabelItemName = WidgetManager::createLabel(itemName, 10, itemGradeToTextColor(itemGrade));
    m_itemWidgets.append(pLabelItemName);
    return pLabelItemName;
}

QProgressBar* SmartSearchAccessory::createQualityBar(const int& quality)
{
    QProgressBar* pQualityBar = WidgetManager::createQualityBar(quality, 50, 20);
    m_itemWidgets.append(pQualityBar);
    return pQualityBar;
}

QVBoxLayout* SmartSearchAccessory::createAbilityLayout(const QHash<Ability, int>& abilities)
{
    QVBoxLayout* pAbilityLayout = new QVBoxLayout();
    m_itemLayouts.append(pAbilityLayout);

    for (auto iter = abilities.begin(); iter != abilities.end(); iter++)
    {
        QLabel* pLabelAbility = WidgetManager::createLabel(QString("%1 +%2").arg(abilityToQString(iter.key())).arg(iter.value()));
        pAbilityLayout->addWidget(pLabelAbility);
        m_itemWidgets.append(pLabelAbility);
    }

    return pAbilityLayout;
}

QVBoxLayout* SmartSearchAccessory::createEngraveLayout(const Accessory* pAccessory)
{
    QVBoxLayout* pEngraveLayout = new QVBoxLayout();
    m_itemLayouts.append(pEngraveLayout);

    const QStringList& engraves = pAccessory->getEngraves();
    for (const QString& engrave : engraves)
    {
        QLabel* pLabelEngrave = WidgetManager::createLabel(QString("%1 +%2").arg(engrave).arg(pAccessory->getEngraveValue(engrave)));
        pEngraveLayout->addWidget(pLabelEngrave);
        m_itemWidgets.append(pLabelEngrave);
    }

    const QStringList& penalties = pAccessory->getPenalties();
    for (const QString& penalty : penalties)
    {
        QLabel* pLabelEngrave = WidgetManager::createLabel(QString("%1 +%2").arg(penalty).arg(pAccessory->getPenaltyValue(penalty)), 10, "red");
        pEngraveLayout->addWidget(pLabelEngrave);
        m_itemWidgets.append(pLabelEngrave);
    }

    return pEngraveLayout;
}

QLabel* SmartSearchAccessory::createLabelPrice(const Price& price)
{
    QLabel* pLabelPrice = nullptr;
    if (price.buyPrice != 0)
        pLabelPrice = WidgetManager::createLabel(QString("%L1\n(%L2)").arg(price.buyPrice).arg(price.bidStartPrice), 10, "", 200, 50);
    else
        pLabelPrice = WidgetManager::createLabel(QString("%L1\n(%L2)").arg("-").arg(price.bidStartPrice), 10, "", 200, 50);
    m_itemWidgets.append(pLabelPrice);
    return pLabelPrice;
}
