#include "smart_search_tripod.h"
#include "ui_smart_search_tripod.h"
#include "ui/widget_manager.h"
#include "resource/resource_manager.h"
#include "game/skill/skill_manager.h"
#include "api/api_manager.h"
#include "api/search_option.h"

#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QIntValidator>

SmartSearchTripod::SmartSearchTripod(QLayout *pLayout) :
    ui(new Ui::SmartSearchTripod),
    m_filterPrice(0),
    m_pPriceValidator(new QIntValidator())
{
    ui->setupUi(this);
    ui->hLayoutMenu->setAlignment(Qt::AlignHCenter);

    pLayout->addWidget(this);
    this->hide();

    initializeClassSelector();
    initializePriceFilter();
    initializeSearchButton();
    initializeResultUI();
}

SmartSearchTripod::~SmartSearchTripod()
{
    clearResult();
    delete m_pClassSelector;
    for (QWidget* pWidget : m_widgets)
        delete pWidget;
    for (QLayout* pLayout : m_layouts)
        delete pLayout;
    delete ui;
}

void SmartSearchTripod::refresh()
{

}

void SmartSearchTripod::initializeClassSelector()
{
    // 직업 목록 초기화
    QStringList classes;
    QJsonArray jsonClasses = ResourceManager::getInstance()->loadJson("class").find("Class")->toArray();

    for (const QJsonValue& value : jsonClasses)
    {
        const QJsonObject &cls = value.toObject();

        classes << cls.find("Child")->toVariant().toStringList();
    }

    // 직업 선택 UI 추가
    QGroupBox *pGroupClassSelector = WidgetManager::createGroupBox("직업 선택");
    pGroupClassSelector->setMaximumWidth(9 + 200 + 9);
    ui->hLayoutMenu->addWidget(pGroupClassSelector);
    m_widgets.append(pGroupClassSelector);

    QHBoxLayout *pLayout = new QHBoxLayout();
    pGroupClassSelector->setLayout(pLayout);
    m_layouts.append(pLayout);

    m_pClassSelector = WidgetManager::createComboBox(classes);
    pLayout->addWidget(m_pClassSelector);
}

void SmartSearchTripod::initializePriceFilter()
{
    // 가격 필터링 UI 추가
    QGroupBox *pGroupPriceFilter = WidgetManager::createGroupBox("가격 필터");
    pGroupPriceFilter->setMaximumWidth(9 + 200 + 9);
    ui->hLayoutMenu->addWidget(pGroupPriceFilter);
    m_widgets.append(pGroupPriceFilter);

    QHBoxLayout *pLayout = new QHBoxLayout();
    pGroupPriceFilter->setLayout(pLayout);
    m_layouts.append(pLayout);

    QLineEdit *pPriceFilter = WidgetManager::createLineEdit(m_pPriceValidator, "금액 입력");
    pLayout->addWidget(pPriceFilter);
    m_widgets.append(pPriceFilter);

    // 값 입력 시 필터링 금액 업데이트
    connect(pPriceFilter, &QLineEdit::textChanged, this, [&](const QString &price){
        m_filterPrice = price.toInt();

        // 입력된 금액 이상이면 색상 변경하여 강조
        const QList<int> &skillCodes = m_tripodPriceLabels.keys();

        for (int skillCode : skillCodes)
        {
            const QHash<int, QLabel*> &tripodPriceLabels = m_tripodPriceLabels[skillCode];

            for (QLabel* pLabel : tripodPriceLabels)
            {
                QString text = pLabel->text();

                if (text.contains("1레벨") || text.contains("없음"))
                    continue;

                int price = text.remove(",").toInt();
                if (price >= m_filterPrice)
                    pLabel->setStyleSheet(LABEL_STYLE_EMPHASIS);
                else
                    pLabel->setStyleSheet(LABEL_STYLE_NORMAL);
            }
        }
    });
}

void SmartSearchTripod::initializeSearchButton()
{
    QPushButton *pSearchButton = WidgetManager::createPushButton("검색");
    ui->hLayoutMenu->addWidget(pSearchButton);
    m_widgets.append(pSearchButton);

    // 검색 버튼 기능 구현
    connect(pSearchButton, &QPushButton::released, this, [&](){
        // 이전 데이터 삭제
        clearResult();

        // 선택된 직업의 스킬 목록 로드 & 트라이포드별로 가격 검색 시작
        const QList<Skill> &skills = SkillManager::getInstance()->skillList(m_pClassSelector->currentText());

        for (int i = 0; i < skills.size(); i++)
        {
            const Skill &skill = skills[i];

            addSkillWidget(skill, (i * 2) + 1);

            const QList<Tripod> &tripods = skill.tripods();

            for (int j = 0; j < tripods.size(); j++)
            {
                const Tripod &tripod = tripods[j];

                addTripodWidget(tripod, skill.skillCode(), (i * 2) + 2, j + (j / 3) + 2);
                searchTripod(skill.skillCode(), tripod.tripodCode());
            }
        }
    });
}

void SmartSearchTripod::initializeResultUI()
{
    const QStringList attributes = {"스킬", "트라이포드 I", "트라이포드 II", "트라이포드 III"};
    const int colSpans[] = {1, 3, 3, 2};

    for (int i = 0, col = 0; i < attributes.size(); i++)
    {
        QLabel *pLabelAttribute = WidgetManager::createLabel(attributes[i], 12);
        ui->gridResult->addWidget(pLabelAttribute, 0, col, 1, colSpans[i], Qt::AlignHCenter);
        m_widgets.append(pLabelAttribute);

        col += colSpans[i];

        if (i < attributes.size() - 1)
        {
            QFrame *pVLine = WidgetManager::createLine(QFrame::VLine);
            ui->gridResult->addWidget(pVLine, 0, col, -1, 1);
            m_widgets.append(pVLine);

            col += 1;
        }
    }
}

void SmartSearchTripod::clearResult()
{
    m_tripodPriceLabels.clear();

    for (QWidget* pWidget : m_resultWidgets)
        delete pWidget;
    m_resultWidgets.clear();

    for (QLayout* pLayout : m_resultLayouts)
        delete pLayout;
    m_resultLayouts.clear();
}

void SmartSearchTripod::searchTripod(int skillCode, int tripodCode)
{
    if (tripodCode == -1)
        return;

    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&, skillCode, tripodCode](QNetworkReply *pReply){
        QJsonDocument response = QJsonDocument::fromJson(pReply->readAll());
        if (response.isNull())
            return;

        const QJsonArray &items = response.object().find("Items")->toArray();
        if (items.size() == 0)
        {
            m_tripodPriceLabels[skillCode][tripodCode]->setText("검색 결과 없음");
            return;
        }

        // 가격 parsing
        const QJsonObject &item = items[0].toObject();
        int price = item.find("AuctionInfo")->toObject().find("BuyPrice")->toInt();

        // UI Update
        m_tripodPriceLabels[skillCode][tripodCode]->setText(QString("%L1").arg(price));
        if (price >= m_filterPrice)
            m_tripodPriceLabels[skillCode][tripodCode]->setStyleSheet(LABEL_STYLE_EMPHASIS);
    });
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    SearchOption searchOption(SearchType::Auction);
    searchOption.setCategoryCode(CategoryCode::Tripod);
    searchOption.setItemGrade(ItemGrade::유물);
    searchOption.setItemTier(3);
    searchOption.setPageNo(1);
    searchOption.setSortCondition("ASC");
    searchOption.setSkillOption(skillCode, tripodCode, 5, 5);

    ApiManager::getInstance()->post(pNetworkManager, LostarkApi::Auction, QJsonDocument(searchOption.toJsonObject()).toJson());
}

void SmartSearchTripod::addSkillWidget(const Skill &skill, int row)
{
    // 구분선 추가
    QFrame *pHLine = WidgetManager::createLine(QFrame::HLine);
    ui->gridResult->addWidget(pHLine, row++, 0, 1, -1);
    m_resultWidgets.append(pHLine);

    QVBoxLayout *pSkillLayout = new QVBoxLayout();
    ui->gridResult->addLayout(pSkillLayout, row, 0);
    m_resultLayouts.append(pSkillLayout);

    // 스킬 아이콘 추가
    QLabel *pIcon = WidgetManager::createIcon(skill.iconPath(), nullptr);
    pSkillLayout->addWidget(pIcon);
    pSkillLayout->setAlignment(pIcon, Qt::AlignHCenter);
    m_resultWidgets.append(pIcon);

    // 스킬명 추가
    QLabel *pLabelSkillName = WidgetManager::createLabel(skill.skillName());
    pSkillLayout->addWidget(pLabelSkillName, Qt::AlignHCenter);
    m_resultWidgets.append(pLabelSkillName);
}

void SmartSearchTripod::addTripodWidget(const Tripod &tripod, int skillCode, int row, int col)
{
    QVBoxLayout *pTripodLayout = new QVBoxLayout();
    ui->gridResult->addLayout(pTripodLayout, row, col);
    m_resultLayouts.append(pTripodLayout);

    // 트라이포드 아이콘 추가
    QLabel *pIcon = WidgetManager::createIcon(tripod.iconPath(), nullptr, "black");
    pTripodLayout->addWidget(pIcon);
    pTripodLayout->setAlignment(pIcon, Qt::AlignHCenter);
    m_resultWidgets.append(pIcon);

    // 트라이포드명 추가
    QLabel *pLabelTripodName = WidgetManager::createLabel(tripod.tripodName());
    pTripodLayout->addWidget(pLabelTripodName);
    m_resultWidgets.append(pLabelTripodName);

    // 트라이포드 가격 레이블 추가
    QLabel *pLabelTripodPrice = nullptr;
    if (tripod.maxLevel() == 1)
        pLabelTripodPrice = WidgetManager::createLabel("1레벨 최대", 10, "", 100, 30);
    else
    {
        pLabelTripodPrice = WidgetManager::createLabel("-", 12, "", 100, 30);
        pLabelTripodPrice->setStyleSheet(LABEL_STYLE_NORMAL);
    }
    pTripodLayout->addWidget(pLabelTripodPrice);
    m_resultWidgets.append(pLabelTripodPrice);
    m_tripodPriceLabels[skillCode][tripod.tripodCode()] = pLabelTripodPrice;
}

