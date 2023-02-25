#include "skill_info_widget.h"
#include "ui_skill_info_widget.h"
#include "ui/font_manager.h"
#include "ui/widget_manager.h"
#include "api/api_manager.h"
#include "api/search_option.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>

SkillInfoWidget::SkillInfoWidget(QWidget* pParent, const SkillInfo& skill, const QList<TripodInfo>& tripods) :
    QWidget(pParent),
    ui(new Ui::SkillInfoWidget),
    m_skillInfo(skill),
    m_tripods(tripods),
    m_labels(0),
    m_layouts(0)
{
    ui->setupUi(this);

    setFonts();
    setAlignments();
    addSkill();
    addTripods();
}

SkillInfoWidget::~SkillInfoWidget()
{
    for (QLabel* pLabel : m_labels)
        delete pLabel;
    for (QLabel* pLabel : m_priceLabels)
        delete pLabel;
    for (QBoxLayout* pLayout : m_layouts)
        delete pLayout;
    delete ui;
}

void SkillInfoWidget::setPriceEmphasis(int price)
{
    QString emphasisStyle = "QLabel { color: %1 }";

    for (QLabel* pPriceLabel : m_priceLabels)
    {
        QString priceText = pPriceLabel->text().remove(",");
        if (price <= priceText.sliced(1, priceText.size() - 3).toInt())
        {
            pPriceLabel->setStyleSheet(emphasisStyle.arg("red"));
        }
        else
        {
            pPriceLabel->setStyleSheet(emphasisStyle.arg("black"));
        }
    }
}

void SkillInfoWidget::setFonts()
{
    FontManager* pFontManager = FontManager::getInstance();
    QFont nanumBold10 = pFontManager->getFont(FontFamily::NanumSquareNeoBold, 10);

    ui->groupTripod1->setFont(nanumBold10);
    ui->groupTripod2->setFont(nanumBold10);
    ui->groupTripod3->setFont(nanumBold10);
}

void SkillInfoWidget::setAlignments()
{
    ui->vLayoutSkill->setAlignment(Qt::AlignVCenter);
}

void SkillInfoWidget::addSkill()
{
    const QString iconPath = ":/skill/image/skill/" + m_skillInfo.iconPath;

    QLabel* pSkillIcon = WidgetManager::createIcon(iconPath, nullptr, "#000000");
    m_labels.append(pSkillIcon);
    ui->vLayoutSkill->addWidget(pSkillIcon);
    ui->vLayoutSkill->setAlignment(pSkillIcon, Qt::AlignHCenter);

    QLabel* pSkillName = WidgetManager::createLabel(m_skillInfo.name, 10, "", LABEL_WIDTH, LABEL_HEIGHT);
    m_labels.append(pSkillName);
    ui->vLayoutSkill->addWidget(pSkillName);
}

void SkillInfoWidget::addTripods()
{
    QString iconPath = ":/tripod/image/tripod/Tripod_Tier_%1_%2.png";

    // 1트포
    for (int i = 0; i < 3; i++)
    {
        const TripodInfo& tripod = m_tripods[i];

        QVBoxLayout* pVLayout = new QVBoxLayout();
        m_layouts.append(pVLayout);
        ui->hLayoutTripod1->addLayout(pVLayout);

        QLabel* pTripodIcon = WidgetManager::createIcon(iconPath.arg(1).arg(tripod.iconIndex), nullptr, "#000000");
        m_labels.append(pTripodIcon);
        pVLayout->addWidget(pTripodIcon);
        pVLayout->setAlignment(pTripodIcon, Qt::AlignHCenter);

        QLabel* pTripodName = WidgetManager::createLabel(tripod.name, 10, "", LABEL_WIDTH, LABEL_HEIGHT);
        m_labels.append(pTripodName);
        pVLayout->addWidget(pTripodName);

        QLabel* pTripodPrice = WidgetManager::createLabel("-", 10, "", LABEL_WIDTH, LABEL_HEIGHT);
        pVLayout->addWidget(pTripodPrice);

        if (tripod.auctionCode == -1)
        {
            pTripodPrice->setText("[1레벨 MAX]");
            m_labels.append(pTripodPrice);
        }
        else
        {
            setTripodPrice(tripod.auctionCode, pTripodPrice);
            m_priceLabels.append(pTripodPrice);
        }
    }

    // 2트포
    for (int i = 3; i < 6; i++)
    {
        const TripodInfo& tripod = m_tripods[i];

        QVBoxLayout* pVLayout = new QVBoxLayout();
        pVLayout->setAlignment(Qt::AlignHCenter);
        m_layouts.append(pVLayout);
        ui->hLayoutTripod2->addLayout(pVLayout);

        QLabel* pTripodIcon = WidgetManager::createIcon(iconPath.arg(2).arg(tripod.iconIndex), nullptr, "#000000");
        m_labels.append(pTripodIcon);
        pVLayout->addWidget(pTripodIcon);
        pVLayout->setAlignment(pTripodIcon, Qt::AlignHCenter);

        QLabel* pTripodName = WidgetManager::createLabel(tripod.name, 10, "", LABEL_WIDTH, LABEL_HEIGHT);
        m_labels.append(pTripodName);
        pVLayout->addWidget(pTripodName);

        QLabel* pTripodPrice = WidgetManager::createLabel("-", 10, "", LABEL_WIDTH, LABEL_HEIGHT);
        pVLayout->addWidget(pTripodPrice);

        if (tripod.auctionCode == -1)
        {
            pTripodPrice->setText("[1레벨 MAX]");
            m_labels.append(pTripodPrice);
        }
        else
        {
            setTripodPrice(tripod.auctionCode, pTripodPrice);
            m_priceLabels.append(pTripodPrice);
        }
    }

    // 3트포
    for (int i = 6; i < 8; i++)
    {
        const TripodInfo& tripod = m_tripods[i];

        QVBoxLayout* pVLayout = new QVBoxLayout();
        pVLayout->setAlignment(Qt::AlignHCenter);
        m_layouts.append(pVLayout);
        ui->hLayoutTripod3->addLayout(pVLayout);

        QLabel* pTripodIcon = WidgetManager::createIcon(iconPath.arg(3).arg(tripod.iconIndex), nullptr, "#000000");
        m_labels.append(pTripodIcon);
        pVLayout->addWidget(pTripodIcon);
        pVLayout->setAlignment(pTripodIcon, Qt::AlignHCenter);

        QLabel* pTripodName = WidgetManager::createLabel(tripod.name, 10, "", LABEL_WIDTH, LABEL_HEIGHT);
        m_labels.append(pTripodName);
        pVLayout->addWidget(pTripodName);

        QLabel* pTripodPrice = WidgetManager::createLabel("-", 10, "", LABEL_WIDTH, LABEL_HEIGHT);
        pVLayout->addWidget(pTripodPrice);

        if (tripod.auctionCode == -1)
        {
            pTripodPrice->setText("[1레벨 MAX]");
            m_labels.append(pTripodPrice);
        }
        else
        {
            setTripodPrice(tripod.auctionCode, pTripodPrice);
            m_priceLabels.append(pTripodPrice);
        }
    }
}

void SkillInfoWidget::setTripodPrice(int tripodCode, QLabel* pLabelPrice)
{
    QNetworkAccessManager* pNetworkManager = new QNetworkAccessManager();
    connect(pNetworkManager, &QNetworkAccessManager::finished, this, [&, pLabelPrice](QNetworkReply* pReply){
        QJsonDocument result = QJsonDocument::fromJson(pReply->readAll());
        if (result.isNull())
        {
            pLabelPrice->setText("[ ? ]");
            return;
        }
        QJsonObject jObjresult = result.object();

        QJsonArray items = jObjresult.find("Items")->toArray();
        if (items.size() == 0)
        {
            pLabelPrice->setText("[ - ]");
        }
        else
        {
            int price = items[0].toObject().find("AuctionInfo")->toObject().find("BuyPrice")->toInt();
            pLabelPrice->setText(QString("[%L1골]").arg(price));
        }
    });
    connect(pNetworkManager, &QNetworkAccessManager::finished, pNetworkManager, &QNetworkAccessManager::deleteLater);

    // build search option
    SearchOption searchOption(SearchType::Auction);
    searchOption.setCategoryCode(CategoryCode::Tripod);
    searchOption.setItemTier(3);
    searchOption.setItemGrade(ItemGrade::유물);
    searchOption.setPageNo(1);
    searchOption.setSortCondition("ASC");
    searchOption.setSkillOption(m_skillInfo.auctionCode, tripodCode, 5);

    ApiManager::getInstance()->post(pNetworkManager, LostarkApi::Auction, QJsonDocument(searchOption.toJsonObject()).toJson());
}
