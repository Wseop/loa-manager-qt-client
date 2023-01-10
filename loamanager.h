#ifndef LOAMANAGER_H
#define LOAMANAGER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class LoaManager; }
QT_END_NAMESPACE

class LoaManager : public QWidget
{
    Q_OBJECT

public:
    LoaManager(QWidget *parent = nullptr);
    ~LoaManager();

private:
    Ui::LoaManager *ui;
};
#endif // LOAMANAGER_H
