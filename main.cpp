#include "loamanager.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoaManager w;
    w.show();
    return a.exec();
}
