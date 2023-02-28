#include "smart_search_accessory.h"
#include "ui_smart_search_accessory.h"
#include "ui/widget_manager.h"
#include "ui/engrave_selector.h"
#include "game_data/character/profile/enum/ability.h"
#include "game_data/character/engrave/engrave_manager.h"
#include "api/api_manager.h"

#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QProgressBar>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QFrame>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <algorithm>

SmartSearchAccessory::SmartSearchAccessory(QLayout* pLayout) :
    ui(new Ui::SmartSearchAccessory),
    m_currentLayoutRows({0, 0, 0})
{
    ui->setupUi(this);
    pLayout->addWidget(this);
    this->hide();

    ui->vLayoutMain->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    initOption();
    initOptionSelector();
    initSearchOptionCode();
    initSearchButton();
    initResultLayout();
}

SmartSearchAccessory::~SmartSearchAccessory()
{
    for (QWidget* pWidget : m_widgets)
        delete pWidget;
    for (EngraveSelector* pEngraveSelector : m_engraveSelectors)
        delete pEngraveSelector;
    for (QWidget* pWidget : m_optionSelectors)
        delete pWidget;
    for (QSpacerItem* pSpacer : m_spacers)
        delete pSpacer;
    clearResult();
    delete ui;
}

void SmartSearchAccessory::refresh()
{
    // Nothing to do
}

void SmartSearchAccessory::initOption()
{
    // 등급
    QStringList grades = {"유물", "고대"};
    m_optionItems << grades;

    // 부위
    QStringList parts = {"목걸이", "귀걸이", "반지"};
    m_optionItems << parts;

    // 특성
    QStringList abilities;
    for (int i = 0; i < static_cast<int>(Ability::Size); i++)
        abilities << abilityToStr(static_cast<Ability>(i));
    for (int i = 0; i < 2; i++)
        m_optionItems << abilities;

    // 품질
    QStringList qualities;
    for (int i = 0; i < 10; i++)
        qualities << QString("%1 이상").arg(i * 10);
    m_optionItems << qualities;

    // 각인
    for (int i = 0; i < 2; i++)
    {
        QStringList dummy;
        m_engraveSelectors << new EngraveSelector(true);
        m_optionItems << dummy;
    }

    // penalty
    QStringList penalties = {"선택 안함", "공격력 감소", "공격속도 감소", "이동속도 감소", "방어력 감소"};
    m_optionItems << penalties;
}

void SmartSearchAccessory::initOptionSelector()
{
    const QStringList labelTexts = {"등급", "부위", "특성1", "특성2", "품질", "각인1 (3)", "각인2 (3~6)", "감소"};
    const QList<int> labelWidths = {50, 50, 50, 50, 50, 50, 75, 50};

    for (int i = 0; i < labelTexts.size(); i++)
    {
        QLabel* pLabel = WidgetManager::createLabel(labelTexts[i], 10, "", labelWidths[i]);
        ui->hLayoutOption->addWidget(pLabel);
        m_widgets.append(pLabel);

        // 각인 선택은 QPushButton
        if (i == OptionIndex::Engrave1 || i == OptionIndex::Engrave2)
        {
            QPushButton* pButtonEngrave = WidgetManager::createPushButton("각인 선택", 10, 100, 25);
            ui->hLayoutOption->addWidget(pButtonEngrave);
            connect(pButtonEngrave, &QPushButton::released, this, [&, i](){
                m_engraveSelectors[i - OptionIndex::Engrave1]->show();
            });
            m_optionSelectors.append(pButtonEngrave);

            // EngraveSelector와 연동
            QList<QPushButton*> buttons = m_engraveSelectors[i - OptionIndex::Engrave1]->getButtons();
            for (QPushButton* pButton : buttons)
            {
                // EngraveSelector에서 각인을 선택하면 pButtonEngrave에 반영
                connect(pButton, &QPushButton::released, this, [&, pButton, pButtonEngrave, i](){
                    pButtonEngrave->setText(pButton->text());
                    m_engraveSelectors[i - OptionIndex::Engrave1]->hide();
                });
            }
        }
        // 그 외 옵션 선택은 ComboBox
        else
        {
            QComboBox* pSelector = WidgetManager::createComboBox(m_optionItems[i]);
            ui->hLayoutOption->addWidget(pSelector);
            m_optionSelectors.append(pSelector);

            // 목걸이를 선택한 경우만 특성2 활성화
            if (i == OptionIndex::Part)
            {
                connect(pSelector, &QComboBox::currentIndexChanged, this, [&](int index){
                    if (index != 0)
                        m_optionSelectors[OptionIndex::Ability2]->setDisabled(true);
                    else if (index == 0)
                        m_optionSelectors[OptionIndex::Ability2]->setEnabled(true);
                });
            }
        }

        QSpacerItem* pSpacer = new QSpacerItem(25, 0);
        ui->hLayoutOption->addSpacerItem(pSpacer);
        m_spacers.append(pSpacer);
    }
}

void SmartSearchAccessory::initSearchOptionCode()
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

void SmartSearchAccessory::initSearchButton()
{
    QPushButton* pSearchButton = WidgetManager::createPushButton("검색", 10);
    ui->hLayoutOption->addWidget(pSearchButton);
    m_widgets.append(pSearchButton);

    // 버튼 기능 구현
    connect(pSearchButton, &QPushButton::released, this, [&](){
        // 각인1 or 각인2가 선택되지 않은 경우 message popup
        for (int i = OptionIndex::Engrave1; i <= OptionIndex::Engrave2; i++)
        {
            QPushButton* pEngraveSelector = dynamic_cast<QPushButton*>(m_optionSelectors[i]);
            if (pEngraveSelector->text() == "각인 선택")
            {
                QMessageBox msgBox;
                msgBox.setText("각인을 모두 선택해주세요.");
                msgBox.exec();
                return;
            }
        }

        clearResult();

        // 검색 옵션 세팅
        QString itemGrade = "";
        ItemType itemType = ItemType::Size;
        SearchOption baseSearchOption(SearchType::Auction);
        baseSearchOption.setItemTier(3);
        baseSearchOption.setPageNo(1);
        baseSearchOption.setSortCondition("ASC");

        for (int i = 0; i < m_optionItems.size(); i++)
        {
            // 각인1 - 수치3으로 고정
            if (i == OptionIndex::Engrave1)
            {
                const QString& engrave = dynamic_cast<QPushButton*>(m_optionSelectors[i])->text();
                baseSearchOption.setEtcOption(ENGRAVE_OPTION_CODE, EngraveManager::getInstance()->getEngraveCode(engrave), 3, 3);
            }
            else if (i != OptionIndex::Engrave2)
            {
                const QComboBox* pSelector = dynamic_cast<QComboBox*>(m_optionSelectors[i]);
                const QString& selectedText = pSelector->currentText();

                if (i == OptionIndex::Grade)
                {
                    baseSearchOption.setItemGrade(strToItemGrade(selectedText));
                    itemGrade = selectedText;
                }
                else if (i == OptionIndex::Part)
                {
                    baseSearchOption.setCategoryCode(m_partCodes[selectedText]);
                    itemType = strToItemType(selectedText);
                }
                else if (i == OptionIndex::Ability1)
                    baseSearchOption.setEtcOption(ABILITY_OPTION_CODE, m_abilityCodes[selectedText]);
                // 특성2는 목걸이를 선택한 경우에만 사용
                else if (i == OptionIndex::Ability2 && dynamic_cast<QComboBox*>(m_optionSelectors[OptionIndex::Part])->currentText() == "목걸이")
                    baseSearchOption.setEtcOption(ABILITY_OPTION_CODE, m_abilityCodes[selectedText]);
                else if (i == OptionIndex::Quality)
                    baseSearchOption.setQuality(selectedText.chopped(3).toInt());
                else if (i == OptionIndex::Penalty && selectedText != "선택 안함")
                    baseSearchOption.setEtcOption(ENGRAVE_OPTION_CODE, EngraveManager::getInstance()->getEngraveCode(selectedText));
            }
        }

        // 각인2의 가능한 모든 수치에 대해 검색 시작
        // 유물 3~5, 고대 4~6
        for (int i = 0; i < 3; i++)
        {
            SearchOption searchOption = baseSearchOption;
            int value = itemGrade == "유물" ? (i + 3) : (i + 4);

            const QString& engrave = dynamic_cast<QPushButton*>(m_optionSelectors[OptionIndex::Engrave2])->text();
            searchOption.setEtcOption(ENGRAVE_OPTION_CODE, EngraveManager::getInstance()->getEngraveCode(engrave), value, value);

            search(searchOption, itemType);
        }
    });
}

void SmartSearchAccessory::initResultLayout()
{
    m_resultLayouts = {ui->gridResult1, ui->gridResult2, ui->gridResult3};
    for (int i = 0; i < m_resultLayouts.size(); i++)
        m_resultLayouts[i]->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    const QStringList attributes = {"#", "아이템명", "품질", "특성", "각인(3/%1,%2)", "가격"};

    for (int i = 0; i < m_resultLayouts.size(); i++)
    {
        for (int j = 0; j < attributes.size(); j++)
        {
            const QString& attribute = attributes[j];

            QLabel* pLabel = nullptr;
            if (j == 4)
                pLabel = WidgetManager::createLabel(attribute.arg(j - 1 + i).arg(j + i), 10, "", LABEL_WIDTHS[j]);
            else
                pLabel = WidgetManager::createLabel(attribute, 10, "", LABEL_WIDTHS[j]);
            m_resultLayouts[i]->addWidget(pLabel, 0, j);
            m_widgets.append(pLabel);
        }
    }
}

void SmartSearchAccessory::search(SearchOption& searchOption, ItemType itemType)
{
    QNetworkAccessManager* pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&](QNetworkReply* pReply){
        // 검색 결과 parsing
        QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
        if (response.isNull())
            return;

        const QJsonArray& items = response.object().find("Items")->toArray();
        for (const QJsonValue& value : items)
        {
            const QJsonObject& item = value.toObject();

            Accessory acc(itemType);
            acc.setName(item.find("Name")->toString());
            acc.setGrade(strToItemGrade(item.find("Grade")->toString()));
            acc.setQuality(item.find("GradeQuality")->toInt());
            acc.setIconPath(item.find("Icon")->toString());

            const QJsonArray& options = item.find("Options")->toArray();
            for (const QJsonValue& value : options)
            {
                const QJsonObject& option = value.toObject();

                const QString& type = option.find("Type")->toString();
                const QString& optionName = option.find("OptionName")->toString();
                const int& optionValue = option.find("Value")->toInt();

                if (type == "STAT")
                    acc.addAbility(strToAbility(optionName), optionValue);
                else if (type == "ABILITY_ENGRAVE")
                {
                    if (option.find("IsPenalty")->toBool())
                        acc.addPenalty(optionName, optionValue);
                    else
                        acc.addEngrave(optionName, optionValue);
                }
            }

            int price = item.find("AuctionInfo")->toObject().find("BuyPrice")->toInt();

            addItemToLayout(acc, price);
        }
    });
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    ApiManager::getInstance()->post(pNetworkManager, LostarkApi::Auction, QJsonDocument(searchOption.toJsonObject()).toJson());
}

void SmartSearchAccessory::addItemToLayout(const Accessory& acc, const int& price)
{
    int col = 0;
    int layoutIndex = 0;

    // 각인 값으로 배치될 레이아웃 결정
    QList<PairEngraveValue> engraves = acc.getEngraves();
    std::sort(engraves.begin(), engraves.end(), [&](PairEngraveValue& a, PairEngraveValue& b){
        return a.second < b.second;
    });

    layoutIndex = engraves[1].second - engraves[0].second;
    if (acc.getGrade() == ItemGrade::고대)
        layoutIndex -= 1;

    // HLine 추가
    QFrame* pHLine = new QFrame();
    pHLine->setFrameStyle(QFrame::HLine);
    m_resultLayouts[layoutIndex]->addWidget(pHLine, ++m_currentLayoutRows[layoutIndex], 0, 1, -1);
    m_itemWidgets.append(pHLine);

    QNetworkAccessManager* pIconLoader = new QNetworkAccessManager();
    connect(pIconLoader, &QNetworkAccessManager::finished, pIconLoader, &QNetworkAccessManager::deleteLater);

    // 아이콘 추가
    QLabel* pIcon = WidgetManager::createIcon(acc.getIconPath(), pIconLoader, backgroundColorCode(acc.getGrade()));
    m_resultLayouts[layoutIndex]->addWidget(pIcon, ++m_currentLayoutRows[layoutIndex], col++);
    m_itemWidgets.append(pIcon);

    // 아이템명 추가
    QLabel* pLabelName = WidgetManager::createLabel(acc.getName(), 10, colorCode(acc.getGrade()), LABEL_WIDTHS[col]);
    m_resultLayouts[layoutIndex]->addWidget(pLabelName, m_currentLayoutRows[layoutIndex], col++);
    m_itemWidgets.append(pLabelName);

    // 품질 추가
    QProgressBar* pQualityBar = WidgetManager::createQualityBar(acc.getQuality(), LABEL_WIDTHS[col], 20, 10);
    m_resultLayouts[layoutIndex]->addWidget(pQualityBar, m_currentLayoutRows[layoutIndex], col++);
    m_itemWidgets.append(pQualityBar);

    // 특성 추가
    QVBoxLayout* pLayoutAbility = new QVBoxLayout();
    pLayoutAbility->setSpacing(1);
    m_resultLayouts[layoutIndex]->addLayout(pLayoutAbility, m_currentLayoutRows[layoutIndex], col++);
    m_itemLayouts.append(pLayoutAbility);

    const QMap<Ability, int>& abilities = acc.getAbilities();
    for (auto iter = abilities.begin(); iter != abilities.end(); iter++)
    {
        QLabel* pLabelAbility = WidgetManager::createLabel(QString("%1 +%2").arg(abilityToStr(iter.key())).arg(iter.value()), 10, "", LABEL_WIDTHS[col - 1]);
        pLayoutAbility->addWidget(pLabelAbility);
        m_itemWidgets.append(pLabelAbility);
    }

    // 각인 추가
    QVBoxLayout* pLayoutEngrave = new QVBoxLayout();
    pLayoutEngrave->setSpacing(1);
    m_resultLayouts[layoutIndex]->addLayout(pLayoutEngrave, m_currentLayoutRows[layoutIndex], col++);
    m_itemLayouts.append(pLayoutEngrave);

    for (const PairEngraveValue& engrave : engraves)
    {
        QLabel* pLabelEngrave = WidgetManager::createLabel(QString("%1 %2").arg(engrave.first).arg(engrave.second), 10, "", LABEL_WIDTHS[col - 1]);
        pLayoutEngrave->addWidget(pLabelEngrave);
        m_itemWidgets.append(pLabelEngrave);
    }

    const PairEngraveValue& penalty = acc.getPenalties()[0];
    QLabel* pLabelPenalty = WidgetManager::createLabel(QString("%1 %2").arg(penalty.first).arg(penalty.second), 10, "red", LABEL_WIDTHS[col - 1]);
    pLayoutEngrave->addWidget(pLabelPenalty);
    m_itemWidgets.append(pLabelPenalty);

    // 가격 추가
    QLabel* pLabelPrice = WidgetManager::createLabel(QString("%L1").arg(price), 10, "", LABEL_WIDTHS[col]);
    m_resultLayouts[layoutIndex]->addWidget(pLabelPrice, m_currentLayoutRows[layoutIndex], col++);
    m_itemWidgets.append(pLabelPrice);
}

void SmartSearchAccessory::clearResult()
{
    m_currentLayoutRows = {0, 0, 0};

    for (QWidget* pWidget : m_itemWidgets)
        delete pWidget;
    m_itemWidgets.clear();

    for (QLayout* pLayout : m_itemLayouts)
        delete pLayout;
    m_itemLayouts.clear();
}
