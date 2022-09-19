#include "mergingfilewidget.h"
#include "ui_mergingfilewidget.h"
#include <QApplication>
#include <QtConcurrent>
#include <QFileDialog>
#include <QSettings>
#include <QSqlQuery>
#include <QProcess>
#include <QDebug>

MergingFileWidget::MergingFileWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MergingFileWidget)
{
    ui->setupUi(this);
    ui->comboBox->installEventFilter(this);
    connect(ui->checkBox, SIGNAL(toggled(bool)), this, SIGNAL(toggled(bool)));
    connect(ui->comboBox, SIGNAL(editTextChanged(QString)), this, SIGNAL(editTextChanged(QString)));
    connect(ui->comboBox, SIGNAL(editTextChanged(QString)), this, SLOT(onMergingFileChanged(QString)));
    connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(browseMergingFile()));
    connect(&_getMergingFilesWatcher, SIGNAL(finished()), this, SLOT(updateMergingFiles()));
    initMergingFilesUpdate();
}

MergingFileWidget::MergingFileWidget(const QString &text, const QStringList &baseRoutes, const QString &currentConfig, const QString &dataConfig, QWidget *parent) :
    QWidget(parent),
    _baseRoutes(baseRoutes),
    _currentConfig(currentConfig),
    _dataConfig(dataConfig),
    ui(new Ui::MergingFileWidget)
{
    ui->setupUi(this);
    ui->comboBox->installEventFilter(this);
    ui->checkBox->setText(text);
    connect(ui->checkBox, SIGNAL(toggled(bool)), this, SIGNAL(toggled(bool)));
    connect(ui->comboBox, SIGNAL(editTextChanged(QString)), this, SIGNAL(editTextChanged(QString)));
    connect(ui->comboBox, SIGNAL(editTextChanged(QString)), this, SLOT(onMergingFileChanged(QString)));
    connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(browseMergingFile()));
    connect(&_getMergingFilesWatcher, SIGNAL(finished()), this, SLOT(updateMergingFiles()));
    initMergingFilesUpdate();
}

MergingFileWidget::~MergingFileWidget()
{
    _getMergingFilesWatcher.cancel();
    _getMergingFilesWatcher.waitForFinished();
    delete ui;

}

void MergingFileWidget::browseMergingFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Выбрать файл...", QDir::currentPath(), "SVRF files (*.svrf);;All files (*)");
    if (!fileName.isEmpty()) {
        ui->comboBox->setEditText(fileName);
    }
}

QString MergingFileWidget::currentText()
{
    return ui->comboBox->currentText();
}

bool MergingFileWidget::eventFilter(QObject *watched, QEvent *event)
{
    QComboBox * comboBox = qobject_cast<QComboBox*>(watched);
    if (comboBox) {
        if (event->type() == QEvent::Wheel) {
            event->ignore();
            return true;
        } else if (event->type() == QEvent::FocusIn) {
            comboBox->setFocusPolicy(Qt::WheelFocus);
        } else if (event->type() == QEvent::FocusOut) {
            comboBox->setFocusPolicy(Qt::StrongFocus);
        }
    }
    return QObject::eventFilter(watched, event);
}

QStringList MergingFileWidget::getMergingFiles(const QMap<QString, QString> &technologyToBaseRoute)
{
    QSettings commonConfig(QFileInfo(QApplication::instance()->applicationDirPath()).dir().path().append("/cfg/common.conf"), QSettings::IniFormat);
    const QString reticleRoot = commonConfig.value("Svn/reticleRoot").toString();
    QStringList mergingFiles;

    foreach(QString baseRoute, _baseRoutes) {
        QString technology = technologyToBaseRoute.value(baseRoute);
        QProcess p;
        p.start( "svn list " + QString(reticleRoot).append("/").append(technology).append("/Technology/").append(baseRoute).append("/PCG/LIB/OPC/other_layers/drc/"));
        if (p.waitForStarted() && p.waitForFinished() && p.exitCode() == 0) {
            foreach (QString file, QString(p.readAllStandardOutput()).split('\n').filter(QRegExp(text()))) {
                QString mergingFile = QString(reticleRoot) + "/" + technology + "/Technology/" + baseRoute + "/PCG/LIB/OPC/other_layers/drc/" + file;
                if (svnFileExists(mergingFile)) {
                    mergingFiles << mergingFile;
                }
            }
        }
    }

    if (mergingFiles.isEmpty()) {
        QSettings config(_dataConfig, QSettings::IniFormat);
        foreach (const QString group, config.childGroups().filter(QRegExp("^\\d+$"))) {
            config.beginGroup(group);
            if (config.value("maskLayer").toString().trimmed() == this->text()) {
                QString mergingFile = config.value("svrfPath").toString().trimmed();
                if (QFileInfo::exists(mergingFile)) {
                    mergingFiles << mergingFile;
                }
            }
            config.endGroup();
        }
    }

    return mergingFiles;
}

void MergingFileWidget::initMergingFilesUpdate()
{
    ui->comboBox->clear();
    ui->comboBox->addItem(QString());
    ui->comboBox->setCurrentIndex(0);

    QMap<QString,QString> technologyToBaseRoute;
    QSqlQuery query;
    foreach (QString baseRoute, _baseRoutes) {
        if (!baseRoute.isEmpty()) {
            QString queryText = "SELECT Technology.Name \
                                FROM Technology, BaseRoute \
                                WHERE BaseRoute.Name = \"" + baseRoute + "\" \
                                AND BaseRoute.TechnologyId = Technology.Id";
            if (query.exec(queryText)) {
                query.first();
                technologyToBaseRoute.insert(baseRoute, query.value(0).toString());
            }
        }
    }

    _getMergingFilesWatcher.cancel();
    _getMergingFilesFuture = QtConcurrent::run(this, &MergingFileWidget::getMergingFiles, technologyToBaseRoute);
    _getMergingFilesWatcher.setFuture(_getMergingFilesFuture);


}

bool MergingFileWidget::isChecked()
{
    return ui->checkBox->isChecked();
}

void MergingFileWidget::onMergingFileChanged(const QString &mergingFile)
{
    ui->checkBox->setEnabled(true);
    this->setPalette(QPalette());
    if (mergingFile.isEmpty()) ui->checkBox->setChecked(false);
    else if (!svnFileExists(mergingFile) && !QFileInfo().exists(mergingFile)) {
        QPalette errorPalette;
        errorPalette.setColor(QPalette::Base, QColor(255, 204, 204));
        this->setPalette(errorPalette);
        this->setChecked(false);
        ui->checkBox->setEnabled(false);
    }
}

void MergingFileWidget::setCurrentText(const QString &text)
{
    ui->comboBox->setCurrentText(text);
}

void MergingFileWidget::setChecked(bool checked)
{
    ui->checkBox->setChecked(checked);
}

bool MergingFileWidget::svnFileExists(const QString &svnFilePath)
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

QString MergingFileWidget::text()
{
    return ui->checkBox->text();
}

void MergingFileWidget::updateMergingFile()
{
    QSettings arraysConfig(_currentConfig, QSettings::IniFormat);
    QString baseRoute = arraysConfig.value("baseRoute").toString();
    if (!baseRoute.isEmpty() && ui->comboBox->findText("/" + baseRoute + "/",Qt::MatchContains) != -1) {
        ui->comboBox->setCurrentIndex(ui->comboBox->findText("/" + baseRoute + "/",Qt::MatchContains));
    }
    else {
        ui->comboBox->setCurrentIndex(1);
    }
    if (currentText().isEmpty()) this->setChecked(false);
}

void MergingFileWidget::updateMergingFiles()
{
    QStringList mergingFiles = _getMergingFilesWatcher.result();
    foreach (QString mergingFile, mergingFiles) {
        ui->comboBox->addItem(mergingFile);
    }
    updateMergingFile();
}
