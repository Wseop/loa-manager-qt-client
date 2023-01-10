#include "loamanager.h"
#include "ui_loamanager.h"

LoaManager::LoaManager(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoaManager)
{
    ui->setupUi(this);
}

LoaManager::~LoaManager()
{
    delete ui;
}

