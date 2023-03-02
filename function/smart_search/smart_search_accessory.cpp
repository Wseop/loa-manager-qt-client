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
#include <algorithm>

const int MAX_SEARCH_COUNT = 3;

SmartSearchAccessory::SmartSearchAccessory(QLayout* pLayout) :
    ui(new Ui::SmartSearchAccessory),
    m_responseCount(0),
    m_currentLayoutRows({0, 0, 0})
{
    ui->setupUi(this);

    pLayout->addWidget(this);
    this->hide();

    initializeOption();
    initializeOptionSelector();
    initializeOptionCode();
    initializeSearchButton();
}

SmartSearchAccessory::~SmartSearchAccessory()
{
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
    // Nothing to do
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
    QPushButton* pSearchButton = WidgetManager::createPushButton("검색", 10);
    ui->hLayoutOption->addWidget(pSearchButton);
    m_widgets.append(pSearchButton);

    // 검색 버튼 기능 구현
    connect(pSearchButton, &QPushButton::released, this, [&](){
        // 각인이 선택되지 않은 경우 검색 중단
        for (int i = OptionIndex::Engrave1; i <= OptionIndex::Engrave2; i++)
        {
            QComboBox* pEngraveSelector = m_optionSelectors[i];
            if (pEngraveSelector->currentIndex() == 0)
            {
                QMessageBox msgBox;
                msgBox.setText("각인을 모두 선택해주세요.");
                msgBox.exec();
                return;
            }
        }

        clearResult();

        // 검색 옵션 세팅
        SearchOption baseSearchOption(SearchType::Auction);
        baseSearchOption.setItemTier(3);
        baseSearchOption.setPageNo(1);
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
            else if (i == static_cast<int>(OptionIndex::Engrave1))
                baseSearchOption.setEtcOption(EtcOptionCode::Engrave, EngraveManager::getInstance()->getEngraveCode(selectedOption), 3, 3);
            else if (i == static_cast<int>(OptionIndex::Penalty) && m_optionSelectors[i]->currentIndex() != 0)
                baseSearchOption.setEtcOption(EtcOptionCode::Penalty, EngraveManager::getInstance()->getEngraveCode(selectedOption));
        }

        // 각인2의 가능한 모든 수치에 대해 검색 시작 (유물 3~5, 고대 4~6)
        for (int i = 0; i < MAX_SEARCH_COUNT; i++)
        {
            SearchOption searchOption = baseSearchOption;
            int value = m_optionSelectors[OptionIndex::Grade]->currentIndex() == 0 ? (i + 3) : (i + 4);

            const QString& engrave = m_optionSelectors[OptionIndex::Engrave2]->currentText();
            searchOption.setEtcOption(EtcOptionCode::Engrave, EngraveManager::getInstance()->getEngraveCode(engrave), value, value);

            searchAccessory(searchOption, static_cast<AccessoryPart>(m_optionSelectors[OptionIndex::Part]->currentIndex()));
        }
    });
}

//void SmartSearchAccessory::initResultLayout()
//{
//    m_resultLayouts = {ui->gridResult1, ui->gridResult2, ui->gridResult3};
//    for (int i = 0; i < m_resultLayouts.size(); i++)
//        m_resultLayouts[i]->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

//    const QStringList attributes = {"#", "아이템명", "품질", "특성", "각인(3/%1,%2)", "가격"};

//    for (int i = 0; i < m_resultLayouts.size(); i++)
//    {
//        for (int j = 0; j < attributes.size(); j++)
//        {
//            const QString& attribute = attributes[j];

//            QLabel* pLabel = nullptr;
//            if (j == 4)
//                pLabel = WidgetManager::createLabel(attribute.arg(j - 1 + i).arg(j + i), 10, "", LABEL_WIDTHS[j]);
//            else
//                pLabel = WidgetManager::createLabel(attribute, 10, "", LABEL_WIDTHS[j]);
//            m_resultLayouts[i]->addWidget(pLabel, 0, j);
//            m_widgets.append(pLabel);
//        }
//    }
//}

void SmartSearchAccessory::searchAccessory(SearchOption& searchOption, AccessoryPart part)
{
    QNetworkAccessManager* pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&](QNetworkReply* pReply){
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
            pAccessory->setAccessoryPart(part);
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
            const int& price = item.find("AuctionInfo")->toObject().find("BuyPrice")->toInt();

            m_searchResults.append({pAccessory, price});
        }

        if (m_responseCount == MAX_SEARCH_COUNT)
            refresh();
    });
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    ApiManager::getInstance()->post(pNetworkManager, LostarkApi::Auction, QJsonDocument(searchOption.toJsonObject()).toJson());
}

//void SmartSearchAccessory::addItemToLayout(const Accessory& acc, const int& price)
//{
//    int col = 0;
//    int layoutIndex = 0;

//    // 각인 값으로 배치될 레이아웃 결정
//    QList<PairEngraveValue> engraves = acc.getEngraves();
//    std::sort(engraves.begin(), engraves.end(), [&](PairEngraveValue& a, PairEngraveValue& b){
//        return a.second < b.second;
//    });

//    layoutIndex = engraves[1].second - engraves[0].second;
//    if (acc.getGrade() == ItemGrade::고대)
//        layoutIndex -= 1;

//    // HLine 추가
//    QFrame* pHLine = WidgetManager::createLine(QFrame::HLine);
//    m_resultLayouts[layoutIndex]->addWidget(pHLine, ++m_currentLayoutRows[layoutIndex], 0, 1, -1);
//    m_itemWidgets.append(pHLine);

//    QNetworkAccessManager* pIconLoader = new QNetworkAccessManager();
//    connect(pIconLoader, &QNetworkAccessManager::finished, pIconLoader, &QNetworkAccessManager::deleteLater);

//    // 아이콘 추가
//    QLabel* pIcon = WidgetManager::createIcon(acc.getIconPath(), pIconLoader, backgroundColorCode(acc.getGrade()));
//    m_resultLayouts[layoutIndex]->addWidget(pIcon, ++m_currentLayoutRows[layoutIndex], col++);
//    m_itemWidgets.append(pIcon);

//    // 아이템명 추가
//    QLabel* pLabelName = WidgetManager::createLabel(acc.getName(), 10, colorCode(acc.getGrade()), LABEL_WIDTHS[col]);
//    m_resultLayouts[layoutIndex]->addWidget(pLabelName, m_currentLayoutRows[layoutIndex], col++);
//    m_itemWidgets.append(pLabelName);

//    // 품질 추가
//    QProgressBar* pQualityBar = WidgetManager::createQualityBar(acc.getQuality(), LABEL_WIDTHS[col], 20, 10);
//    m_resultLayouts[layoutIndex]->addWidget(pQualityBar, m_currentLayoutRows[layoutIndex], col++);
//    m_itemWidgets.append(pQualityBar);

//    // 특성 추가
//    QVBoxLayout* pLayoutAbility = new QVBoxLayout();
//    pLayoutAbility->setSpacing(1);
//    m_resultLayouts[layoutIndex]->addLayout(pLayoutAbility, m_currentLayoutRows[layoutIndex], col++);
//    m_itemLayouts.append(pLayoutAbility);

//    const QMap<Ability, int>& abilities = acc.getAbilities();
//    for (auto iter = abilities.begin(); iter != abilities.end(); iter++)
//    {
//        QLabel* pLabelAbility = WidgetManager::createLabel(QString("%1 +%2").arg(abilityToStr(iter.key())).arg(iter.value()), 10, "", LABEL_WIDTHS[col - 1]);
//        pLayoutAbility->addWidget(pLabelAbility);
//        m_itemWidgets.append(pLabelAbility);
//    }

//    // 각인 추가
//    QVBoxLayout* pLayoutEngrave = new QVBoxLayout();
//    pLayoutEngrave->setSpacing(1);
//    m_resultLayouts[layoutIndex]->addLayout(pLayoutEngrave, m_currentLayoutRows[layoutIndex], col++);
//    m_itemLayouts.append(pLayoutEngrave);

//    for (const PairEngraveValue& engrave : engraves)
//    {
//        QLabel* pLabelEngrave = WidgetManager::createLabel(QString("%1 %2").arg(engrave.first).arg(engrave.second), 10, "", LABEL_WIDTHS[col - 1]);
//        pLayoutEngrave->addWidget(pLabelEngrave);
//        m_itemWidgets.append(pLabelEngrave);
//    }

//    const PairEngraveValue& penalty = acc.getPenalties()[0];
//    QLabel* pLabelPenalty = WidgetManager::createLabel(QString("%1 %2").arg(penalty.first).arg(penalty.second), 10, "red", LABEL_WIDTHS[col - 1]);
//    pLayoutEngrave->addWidget(pLabelPenalty);
//    m_itemWidgets.append(pLabelPenalty);

//    // 가격 추가
//    QLabel* pLabelPrice = WidgetManager::createLabel(QString("%L1").arg(price), 10, "", LABEL_WIDTHS[col]);
//    m_resultLayouts[layoutIndex]->addWidget(pLabelPrice, m_currentLayoutRows[layoutIndex], col++);
//    m_itemWidgets.append(pLabelPrice);
//}

void SmartSearchAccessory::clearResult()
{
    m_responseCount = 0;
    m_currentLayoutRows = {0, 0, 0};

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
