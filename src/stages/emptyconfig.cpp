#include "emptyconfig.h"
#include "ui_emptyconfig.h"



// Constructor 1
EmptyConfig::EmptyConfig(const QString &title, QWidget *parent) :
    StageConfig(parent),
    ui(new Ui::EmptyConfig)
{
    ui->setupUi(this);
    ui->_titleL->setText(title);
    setStatus(Status_Empty);
}



// Destructor
EmptyConfig::~EmptyConfig()
{
    delete ui;
}


