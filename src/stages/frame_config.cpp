#include "frame_config.h"
#include "ui_frame_config.h"
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QtConcurrentRun>
#include <QSettings>
#include <QSqlQuery>
#include <QSqlError>

// Constructor 1
Frame_config::Frame_config(const QString &title, QWidget *parent) :
    StageConfig(parent),
    ui(new Ui::Frame_config)
{
    ui->setupUi(this);
    ui->_titleL->setText(title);

    connect(ui->_cadCb, SIGNAL(toggled(bool)), this, SLOT(updateStatus()));
    connect(ui->_maskCb, SIGNAL(toggled(bool)), this, SLOT(updateStatus()));
    connect(ui->_configFileCmb, SIGNAL(editTextChanged(QString)), this, SLOT(onConfigFileChanged(QString)));
    connect(ui->_configFilePb, SIGNAL(clicked()), this, SLOT(browseConfigFile()));

    updateConfigFiles();
    updateStatus();
}



// Destructor
Frame_config::~Frame_config()
{
    delete ui;
}



// Browses config file and sets its path to config file line edit
void Frame_config::browseConfigFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Выбрать файл...", QDir::currentPath(), "Config files (*.conf);;All files (*)");
    if (!fileName.isEmpty()) {
        ui->_configFileCmb->setEditText(fileName);
    }
}


// Returns cad frame check box
QCheckBox* Frame_config::cadCb()
{
    return ui->_cadCb;
}


// Returns config file path
QString Frame_config::configFilePath()
{
    return ui->_configFileCmb->currentText().trimmed();
}


// Returns psow file path
QString Frame_config::psowFilePath()
{
    return QString(_flowDirPath).append("/CONFIG/data.conf");
}


// Returns mask frame check box
QCheckBox* Frame_config::maskCb()
{
    return ui->_maskCb;
}


// Do, when config file path is changed
void Frame_config::onConfigFileChanged(const QString &configFile)
{
    Q_UNUSED(configFile);
    updateStatus();
}



// Do, when flow directory path is changed
void Frame_config::onFlowDirPathChanged(const QString &dirPath)
{
    Q_UNUSED(dirPath);
    updateConfigFiles();
}



void Frame_config::setArgs(const QString &args)
{
    QJsonDocument stageConfig = QJsonDocument::fromJson(args.toUtf8());
    QJsonObject stageGroup = stageConfig.object();

    ui->_cadCb->setChecked(stageGroup["cad"].toBool());
    ui->_maskCb->setChecked(stageGroup["mask"].toBool());
    ui->_configFileCmb->setCurrentText(stageGroup["config"].toString());
}



// Updates frame configuration file
void Frame_config::updateConfigFiles()
{
    ui->_configFileCmb->clear();

    const QString configPath = QString(_flowDirPath).append("/INPUT_DATA/frame.conf");
    if (QFile::exists(configPath)) {
        ui->_configFileCmb->addItem(QFileInfo(configPath).absoluteFilePath());
    }
}



// Updates stage config status
void Frame_config::updateStatus()
{
    QFile configFile(ui->_configFileCmb->currentText().trimmed());

    if ((!ui->_cadCb->isChecked() && !ui->_maskCb->isChecked()) || !QFileInfo(configFile).isFile() || !configFile.exists()) {
        setStatus(Status_Error);
    } else if (ui->_cadCb->isChecked() && ui->_maskCb->isChecked() && QFileInfo(configFile).isFile() && configFile.exists()) {
        setStatus(Status_Ok);
    } else {
        setStatus(Status_Warning);
    }
}


// Returns true if svn path "svnPath" exists, otherwise returns false
bool Frame_config::svnFileExists(const QString &svnFilePath)
{
    QProcess p;
    p.start(QString("svn list ").append(svnFilePath));
    if (p.waitForStarted()) {
        if (p.waitForFinished()) {
            if (p.exitCode() == 0) {
                if (p.readAllStandardOutput().trimmed() == QFileInfo(svnFilePath).fileName()) {
                    return 1;
                }
            }
        }
    }
    return 0;
}
