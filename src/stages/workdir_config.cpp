#include "workdir_config.h"
#include "ui_workdir_config.h"
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QtConcurrentRun>
#include <QSettings>



// Constructor 1
WorkDir_config::WorkDir_config(const QString &title, QWidget *parent) :
    StageConfig(parent),
    ui(new Ui::WorkDir_config)
{
    ui->setupUi(this);
    ui->_titleL->setText(title);

    connect(ui->_baseFramePathLe, SIGNAL(textChanged(QString)), this, SLOT(onBaseFramePathChanged(QString)));
    connect(ui->_baseFrameBrowsePb, SIGNAL(clicked()), this, SLOT(browseBaseFrame()));
}



// Destructor
WorkDir_config::~WorkDir_config()
{
    delete ui;
}



// Returns base frame path
QString WorkDir_config::baseFramePath()
{
    return ui->_baseFramePathLe->text().trimmed();
}



// Browses base frame file and sets path to base frame path line edit
void WorkDir_config::browseBaseFrame()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Выбрать файл...", QDir::currentPath(), "Topology files (*.oas *.oas.gz *.gds *.gds.gz);;All files (*)");
    if (!fileName.isEmpty()) {
        ui->_baseFramePathLe->setText(fileName);
    }
}



// Do, when config file path is changed
void WorkDir_config::onBaseFramePathChanged(const QString &path)
{
    Q_UNUSED(path);
    updateStatus();
}



void WorkDir_config::setArgs(const QString &args)
{
    QJsonDocument stageConfig = QJsonDocument::fromJson(args.toUtf8());
    QJsonObject stageGroup = stageConfig.object();

    ui->_baseFramePathLe->setText(stageGroup["base"].toString());
}



// Updates stage config status
void WorkDir_config::updateStatus()
{
    QFile baseFrameFile(ui->_baseFramePathLe->text().trimmed());
    if (!baseFrameFile.fileName().isEmpty() && !baseFrameFile.exists()) {
        setStatus(Status_Error);
    } else {
        setStatus(Status_Ok);
    }
}


