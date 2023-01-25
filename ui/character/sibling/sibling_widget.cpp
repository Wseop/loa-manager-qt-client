#include "sibling_widget.h"
#include "ui_sibling_widget.h"
#include "ui/widget_manager.h"
#include "function/character_search/character_search.h"
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <algorithm>

SiblingWidget::SiblingWidget(QWidget* pParent, const QList<Other>& sibling) :
    QWidget(pParent),
    ui(new Ui::SiblingWidget),
    m_sibling(sibling)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icon/Home.ico"));
    this->setWindowTitle("보유 캐릭터");

    setServerToSibling();
    addServerGridLayouts();
    addSiblingToGrid();
}

SiblingWidget::~SiblingWidget()
{
    for (QLabel* pLabel : m_labels)
        delete pLabel;
    for (QPushButton* pButton : m_buttons)
        delete pButton;
    for (auto iter = m_serverToGridLayout.begin(); iter != m_serverToGridLayout.end(); iter++)
        delete iter.value();
    delete ui;
}

void SiblingWidget::setServerToSibling()
{
    for (const Other& other : m_sibling)
    {
        m_serverToSibling[other.server].append(other);
    }

    // 아이템 레벨을 기준으로 내림차순 정렬
    const QStringList& servers = m_serverToSibling.keys();
    for (const QString& server : servers)
    {
        QList<Other>& sibling = m_serverToSibling[server];
        std::sort(sibling.begin(), sibling.end(), [&](Other& a, Other& b){
            return a.itemLevel > b.itemLevel;
        });
    }
}

void SiblingWidget::addServerGridLayouts()
{
    const QStringList& servers = m_serverToSibling.keys();
    for (const QString& server : servers)
    {
        QGridLayout* pGridLayout = new QGridLayout();
        m_serverToGridLayout[server] = pGridLayout;
        ui->vLayoutSiblingScrollArea->addLayout(pGridLayout);

        QLabel* pServerLabel = WidgetManager::createLabel("@" + server, 100, 25, 12, this, "#B178EA");
        m_labels.append(pServerLabel);
        pGridLayout->addWidget(pServerLabel, 0, 0, 1, 5);
    }
}

void SiblingWidget::addSiblingToGrid()
{
    const int MAX_COL = 5;

    const QStringList& servers = m_serverToSibling.keys();
    for (const QString& server : servers)
    {
        const QList<Other>& sibling = m_serverToSibling[server];
        for (int i = 0; i < sibling.size(); i++)
        {
            const Other& other = sibling[i];

            QString buttonText;
            buttonText += QString("%1 Lv.%2 [%3]\n").arg(classToStr(other.cls)).arg(other.characterLevel).arg(other.itemLevel);
            buttonText += other.characterName;

            QPushButton* pButton = WidgetManager::createPushButton(buttonText, 200, 50, 10, this);
            m_buttons.append(pButton);
            m_serverToGridLayout[server]->addWidget(pButton, (i / MAX_COL) + 1, i % MAX_COL);

            connect(pButton, &QPushButton::released, this, [&, other](){
                CharacterSearch::getInstance()->changeCharacter(other.characterName);
                this->close();
            });
        }
    }
}
