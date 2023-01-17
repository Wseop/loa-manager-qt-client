#ifndef PROFILE_WIDGET_H
#define PROFILE_WIDGET_H

#include <QWidget>

class Profile;
class QLabel;

namespace Ui {
class ProfileWidget;
}

class ProfileWidget : public QWidget
{
    Q_OBJECT

public:
    ProfileWidget(QWidget* pParent, const Profile* pProfile);
    ~ProfileWidget();

private:
    void addLabels();

private:
    Ui::ProfileWidget *ui;

    const int LABEL_WIDTH = 100;
    const int LABEL_HEIGHT = 25;

    const Profile* m_pProfile;
    QList<QLabel*> m_labels;
};

#endif // PROFILE_WIDGET_H
