#ifndef SKILLRUNE_WIDGET_H
#define SKILLRUNE_WIDGET_H

#include <QWidget>

class SkillRune;
class QLabel;
class QNetworkAccessManager;

namespace Ui {
class SkillRuneWidget;
}

class SkillRuneWidget : public QWidget
{
    Q_OBJECT

public:
    SkillRuneWidget(QWidget* pParent, const SkillRune* pSkillRune);
    ~SkillRuneWidget();

private:
    void addIcon();
    void addLabels();
    void setFonts();

private:
    Ui::SkillRuneWidget *ui;

    const int LABEL_WIDTH = 50;
    const int LABEL_HEIGHT = 25;

    const SkillRune* m_pSkillRune;
    QList<QLabel*> m_labels;
    QNetworkAccessManager* m_pNetworkManager;
};

#endif // SKILLRUNE_WIDGET_H
