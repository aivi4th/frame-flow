#include <QComboBox>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QProcess>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "include/singletone.hpp"
#include "authenticationdialog.h"


typedef OpfshUtils::Singleton<QSqlDatabase> SSqlDatabase;

// Constructor 1
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _outputDialog(0),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _settingsBg.setExclusive(1);
    connectToDatabase();
    _flow = new Flow(ui->_flowDirPathLe->text().trimmed(), this->centralWidget());

    foreach(Stage* stage, _flow->stages()) {
        if (stage) {
            QHBoxLayout* stageL = new QHBoxLayout;
            stageL->setSpacing(9);

            // Force checkbox
            QCheckBox* forceCb = new QCheckBox();
            forceCb->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
            forceCb->setFixedHeight(25);
            stageL->addWidget(forceCb);
            connect(forceCb, SIGNAL(toggled(bool)), stage, SLOT(setForced(bool)));
            connect(forceCb, SIGNAL(toggled(bool)), this, SLOT(updateForceAllCmb()));
            connect(stage, SIGNAL(forced(bool)), forceCb, SLOT(setChecked(bool)));

            // Check checkbox
            QCheckBox* checkCb = new QCheckBox(stage->name());
            checkCb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            checkCb->setFixedHeight(25);
            QFont font;
            font.setPointSize(10);
            font.setWeight(50);
            checkCb->setFont(font);
            stageL->addWidget(checkCb);
            connect(checkCb, SIGNAL(toggled(bool)), stage, SLOT(setChecked(bool)));
            connect(checkCb, SIGNAL(toggled(bool)), this, SLOT(updateCheckAllCmb()));
            connect(stage, SIGNAL(checked(bool)), checkCb, SLOT(setChecked(bool)));

            // Settings pushbutton
            QPushButton* settingsPb = new QPushButton;
            settingsPb->setFixedSize(30, 25);
            settingsPb->setCheckable(1);
            switch (stage->config()->status()) {
            case StageConfig::Status_Empty:
                settingsPb->setIcon(QIcon(":/icons/green_settings"));
                settingsPb->setEnabled(0);
                break;
            case StageConfig::Status_Ok:
                settingsPb->setIcon(QIcon(":/icons/green_settings"));
                break;
            case StageConfig::Status_Warning:
                settingsPb->setIcon(QIcon(":/icons/orange_settings"));
                break;
            case StageConfig::Status_Error:
                settingsPb->setIcon(QIcon(":/icons/red_settings"));
                break;
            }
            _settingsBg.addButton(settingsPb);
            stageL->addWidget(settingsPb);

            ui->_stagesL->addLayout(stageL);

            // Config widget
            StageConfig* config = stage->config();
            ui->_configsSw->addWidget(config);
             _updateSettingsButtonMapper.setMapping(stage->config(), settingsPb);
            connect(config, SIGNAL(statusChanged(StageConfig::Status)), &_updateSettingsButtonMapper, SLOT(map()));
            _showConfigMapper.setMapping(settingsPb, config);
            connect(settingsPb, SIGNAL(toggled(bool)), &_showConfigMapper, SLOT(map()));

            // Reset
            checkCb->setChecked(stage->isChecked());
            forceCb->setChecked(stage->isForced());
            if (!_settingsBg.checkedButton() && settingsPb->isEnabled()) {
                settingsPb->setChecked(1);
            }
        }
    }
    addBaseRoute();

    connect(ui->_actionManual, SIGNAL(triggered()), this, SLOT(showManual()));
    connect(ui->_removeBaseRoutePb, SIGNAL(clicked()), this, SLOT(removeBaseRoute()));
    connect(ui->_addBaseRoutePb, SIGNAL(clicked()), this, SLOT(addBaseRoute()));
    connect(ui->_flowDirPathLe, SIGNAL(textChanged(QString)), _flow, SLOT(setDirPath(QString)));
    connect(ui->_flowDirPathLe, SIGNAL(textChanged(QString)), this, SLOT(loadLastConfiguration()));
    connect(ui->_flowDirBrowsePb, SIGNAL(clicked()), this, SLOT(browseFlowDir()));
    connect(ui->_checkStagesCb, SIGNAL(clicked()), this, SLOT(checkStages()));
    connect(ui->_forceStagesCb, SIGNAL(clicked()), this, SLOT(forceStages()));
    connect(&_showConfigMapper, SIGNAL(mapped(QWidget*)), this, SLOT(showConfig(QWidget*)));
    connect(&_updateSettingsButtonMapper, SIGNAL(mapped(QWidget*)), this, SLOT(updateSettingsButton(QWidget*)));
    connect(ui->_launchPb, SIGNAL(clicked()), this, SLOT(launch()));
    connect(_flow, SIGNAL(statusChanged(Flow::Status)), this, SLOT(onFlowStatusChanged(Flow::Status)));

    updateFlowDirPathCmb();

}



// Destructor
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addBaseRoute(const QString &name)
{
    QComboBox* cmb = new QComboBox;
    cmb->setFixedSize(150, 25);
    cmb->addItem(QString());

    QSqlQuery query;
    QString queryText = "SELECT Name FROM BaseRoute ORDER BY Name ASC";
    if (query.exec(queryText)) {
        while (query.next()) {
            cmb->addItem(query.value(0).toString());
        }
    }
    else {
        qDebug() << query.lastError();
    }

    if (!name.isNull()) {
        cmb->setCurrentIndex(cmb->findText(name));
    }

    ui->_baseRouteLay->addWidget(cmb);
    connect(cmb, SIGNAL(currentTextChanged(QString)), this, SLOT(updateBaseRoutes()));
}

// Browses flow directory and sets its path to line edit
void MainWindow::browseFlowDir()
{
    QString dirName = QFileDialog::getExistingDirectory(this, "Выбор директории...", QDir::currentPath(), QFileDialog::ShowDirsOnly);
    if (!dirName.isEmpty()) {
        ui->_flowDirPathLe->setText(dirName);
    }
}



// Checks all stages
void MainWindow::checkStages()
{
    bool checked;
    if (ui->_checkStagesCb->checkState() == Qt::Checked || ui->_checkStagesCb->checkState() == Qt::PartiallyChecked) {
        checked = 1;
    } else {
        checked = 0;
    }
    for (int i = 0; i < ui->_stagesL->count(); i++) {
        QLayout* layout = qobject_cast<QLayout*>(ui->_stagesL->itemAt(i)->layout());
        if (layout) {
            QCheckBox* cb = qobject_cast<QCheckBox*>(layout->itemAt(1)->widget());
            if (cb) {
                if (cb->isEnabled()) {
                    cb->setChecked(checked);
                }
            }
        }
    }
}


// Подключение к базе данных
void MainWindow::connectToDatabase(bool requestCredentials)
{
    const QString commonConfigPath = QApplication::instance()->property("commonConfigPath").toString();
    const QSettings commonConfig(commonConfigPath, QSettings::NativeFormat);
    const QString hostname = commonConfig.value("Sql/host").toString();
    const QString databasename = commonConfig.value("Sql/database").toString();

    const QString userConfigPath = QDir::homePath() + "/." + QApplication::applicationName() + "/user.conf";
    QSettings userConfig(userConfigPath, QSettings::NativeFormat);
    QString username = userConfig.value("Sql/username").toString();
    QString password = userConfig.value("Sql/password").toString();
    if (username.isEmpty() || password.isEmpty()) {
        requestCredentials = true;
    }

    // Запрос данных аутентификации
    if (requestCredentials) {
        QWidget* parent = this->isVisible() ? this : nullptr;
        AuthenticationDialog dialog(parent, username, password);
        dialog.setWindowIcon(QIcon(":/icons/main"));
        int ret = dialog.exec();
        if (ret == QDialog::Rejected) {
            return ;
        }
        username = dialog.username();
        password = dialog.password();
        if (dialog.rememberIsChecked()) {
            userConfig.setValue("Sql/username", username);
            userConfig.setValue("Sql/password", password);
            userConfig.sync();
            QFile::setPermissions(userConfigPath, QFile::WriteUser | QFile::ReadUser);
        }
    }

    // Подключение к базе данных
    QSqlDatabase* database = SSqlDatabase::instance();
    database->close();
    if (QSqlDatabase::connectionNames().isEmpty()) {
        *database = QSqlDatabase::addDatabase("QMYSQL");
    }
    database->setHostName(hostname);
    database->setDatabaseName(databasename);
    database->setUserName(username);
    database->setPassword(password);

    if (!database->open()) {
        const QString title = "Ошибка подключения к базе данных";
        QString text = "Не удалось подключиться к базе данных:\n";
        text.append("host:           " + hostname + "\n");
        text.append("database:   " + databasename + "\n");
        text.append("credentials: " + userConfigPath + "\n");
        this->isVisible() ? QMessageBox::critical(this, title, text) : QMessageBox::critical(nullptr, title, text);
    }
}


// Force all stages
void MainWindow::forceStages()
{
    bool checked;
    if (ui->_forceStagesCb->checkState() == Qt::Checked || ui->_forceStagesCb->checkState() == Qt::PartiallyChecked) {
        checked = 1;
    } else {
        checked = 0;
    }
    for (int i = 0; i < ui->_stagesL->count(); i++) {
        QLayout* layout = qobject_cast<QLayout*>(ui->_stagesL->itemAt(i)->layout());
        if (layout) {
            QCheckBox* cb = qobject_cast<QCheckBox*>(layout->itemAt(0)->widget());
            if (cb) {
                if (cb->isEnabled()) {
                    cb->setChecked(checked);
                }
            }
        }
    }
}



// Launches flow
void MainWindow::launch()
{
    // Сохранение конфигурации запуска
    const QString timestamp = QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss");
    const QString runConfigPath = ui->_flowDirPathLe->text().trimmed() + "/LOGS/run_" + timestamp + ".conf";
    QApplication::instance()->setProperty("runConfigPath", runConfigPath);

    QStringList baseRoutes;
    for(int i = 0; i < ui->_baseRouteLay->count(); i++) {
        QComboBox * baseRouteCmb = qobject_cast<QComboBox*>(ui->_baseRouteLay->itemAt(i)->widget());
        baseRoutes << baseRouteCmb->currentText();
    }

    QJsonObject stageGroup;
    stageGroup.insert("timestamp", timestamp);
    stageGroup.insert("application", QApplication::applicationName() + " " + QApplication::applicationVersion());
    stageGroup.insert("flowDir", ui->_flowDirPathLe->text().trimmed());
    stageGroup.insert("baseRoutes", baseRoutes.join(' '));

    QJsonObject mainGroup;
    mainGroup.insert("General", stageGroup);

    QJsonDocument stageConfig(mainGroup);
    QString jsonString = stageConfig.toJson(QJsonDocument::Indented);

    QFile runConfig;
    runConfig.setFileName(QApplication::instance()->property("runConfigPath").toString());
    runConfig.open(QIODevice::Append | QIODevice::Text);
    QTextStream stream(&runConfig);
    stream << jsonString;
    runConfig.close();

/*
//    QSettings runConfig(runConfigPath, QSettings::IniFormat);
//    runConfig.setIniCodec("UTF-8");
//    runConfig.setValue("timestamp", timestamp);
//    runConfig.setValue("application", QApplication::applicationName() + " " + QApplication::applicationVersion());
//    runConfig.setValue("flowDir", ui->_flowDirPathLe->text().trimmed());
//    runConfig.setValue("baseRoutes", baseRoutes);
//*/


    _flow->initialize();

    bool configError = false;
    foreach (Stage* stage, _flow->stages()) {
        if (stage->isChecked() && stage->config()->status() == StageConfig::Status_Error) {
            configError = true;
            break;
        }
    }

    // Создание директории LOG
    QDir logDir(ui->_flowDirPathLe->text().trimmed() + "/LOGS");
    if (!logDir.mkpath(logDir.path())) {
        QMessageBox::critical(this, QApplication::applicationName(), "Ошибка создания директории \"" + logDir.path() + "\"");
        return ;
    }

    if (baseRoutes.isEmpty() || baseRoutes.join("").isEmpty()) {
        QMessageBox::critical(this, "Ошибка запуска", "Не указано ни одного базового маршрута. Запуск невозможен.");
        _flow->reset();
    } else if (ui->_flowDirPathLe->text().trimmed().isEmpty()) {
        QMessageBox::critical(this, "Ошибка запуска", "Не указана директория FLOW. Запуск невозможен.");
        _flow->reset();
    } else if (configError) {
        QMessageBox::critical(this, "Ошибка запуска", "Ошибка конфигурации запланированных на выполнение этапов. Запуск невозможен.");
        _flow->reset();
    } else if (_flow->status() == Flow::Status_UserSkipped) {
        QMessageBox::critical(this, "Ошибка запуска", "Ни один из этапов не был запланирован на выполнение. Запуск невозможен.");
        _flow->reset();
    } else {
        _outputDialog = new OutputDialog1(_flow, this);
        connect(_outputDialog, SIGNAL(accepted()), this, SLOT(onOutputDialogClosed()));
        connect(_outputDialog, SIGNAL(rejected()), this, SLOT(onOutputDialogClosed()));
        _outputDialog->show();

        _flow->launch();
    }

}

void MainWindow::loadLastConfiguration()
{
    const QString logDirPath = QString(_flow->dirPath()).append("/LOGS");
    const QStringList runConfigNames = QDir(logDirPath).entryList(QStringList("run_*.conf"), QDir::Files, QDir::Name);
    if (!runConfigNames.isEmpty()) {
        const QString lastRunConfigPath = logDirPath + "/" + runConfigNames.last();

        QString jsonString;
        QFile runConfig(lastRunConfigPath);
        runConfig.open(QIODevice::ReadOnly | QIODevice::Text);
        jsonString = runConfig.readAll();
        runConfig.close();

        QJsonDocument runConf = QJsonDocument::fromJson(jsonString.toUtf8());
        QJsonObject mainGroup = runConf.object();

        QJsonValue stageConfig = mainGroup.value("General");
        if (stageConfig.isObject()) {
            QJsonObject stageGroup = stageConfig.toObject();
            setBaseRoutes(stageGroup["baseRoutes"].toString().split(' '));
        }
    }
}

// Do, when flow status is changed
void MainWindow::onFlowStatusChanged(Flow::Status status)
{
    Flow::Result result = _flow->result();
    switch (status) {
    case Flow::Status_AutoSkipped:
        QMessageBox::information(this, "Выполнение завершено", "Все этапы маршрута пропущены, т.к. отсутствует необходимость их запуска.");
        break;
    case Flow::Status_Complited:
        switch (result) {
        case Flow::Result_Ok:
            QMessageBox::information(this, "Выполнение завершено", "Выполнение завершено успешно");
            break;
        case Flow::Result_Warning:
            QMessageBox::warning(this, "Выполнение завершено", "Выполнение завершено c предупреждениями");
            break;
        case Flow::Result_Error:
            QMessageBox::critical(this, "Выполнение завершено", "Выполнение завершено c ошибками");
            break;
        default:
            QMessageBox::critical(this, "Выполнение завершено", "Выполнение завершено c неизвестным результатом");
            break;
        }
        break;
    case Flow::Status_Error:
        QMessageBox::critical(this, "Выполнение завершено", "В процессе выполнения возникла ошибка. Выполнение прервано.");
        break;
    default:
        break;
    }
}



// Do, when output dialog is closed
void MainWindow::onOutputDialogClosed()
{
    delete _outputDialog; _outputDialog = 0;
}

// Shows config widget
void MainWindow::showConfig(QWidget *w)
{
    QPushButton* settingsPb = qobject_cast<QPushButton*>(_showConfigMapper.mapping(w));
    if (settingsPb) {
        if (settingsPb->isChecked()) {
            ui->_configsSw->setCurrentWidget(w);
        }
    }
}

void MainWindow::removeBaseRoute()
{
    if (ui->_baseRouteLay->count() > 0) {
        QWidget* w = ui->_baseRouteLay->takeAt(ui->_baseRouteLay->count() - 1)->widget();
        delete w;
        updateBaseRoutes();
    }
}

void MainWindow::setBaseRoutes(const QStringList &baseRoutes)
{
    while (ui->_baseRouteLay->count() > 0) {
        QWidget* w = ui->_baseRouteLay->takeAt(ui->_baseRouteLay->count() - 1)->widget();
        delete w;
    }
    foreach (QString baseRoute, baseRoutes) {
        addBaseRoute(baseRoute);
    }
    updateBaseRoutes();
}

// Shows manual
void MainWindow::showManual()
{
    /*
    QStringList manuals;
    QDir docDir(QFileInfo(QApplication::applicationDirPath()).dir().path().append("/doc"));
    foreach (QString entry, docDir.entryList()) {
        if (QRegExp("^ui_manual_v\\d+.\\d+.pdf$").exactMatch(entry)) {
            manuals << docDir.path().append("/").append(entry);
        }
    }

    QString command = QString("evince ").append(manuals.last());

    QProcess p;
    if (!p.startDetached(command)) {
        QMessageBox::critical(this, "Ошибка запуска"), "Ошибка запуска команды:\n").append(command));
    }
    */
}



void MainWindow::updateBaseRoutes()
{
    QStringList baseRoutes;
    for(int i = 0; i < ui->_baseRouteLay->count(); i++) {
        QComboBox * baseRouteCmb = qobject_cast<QComboBox*>(ui->_baseRouteLay->itemAt(i)->widget());
        if (! baseRoutes.contains(baseRouteCmb->currentText()))
            baseRoutes << baseRouteCmb->currentText();
    }

    _flow->setBaseRoutesList(baseRoutes);
}



// Updates flow directory path combobox
void MainWindow::updateFlowDirPathCmb()
{
    ui->_flowDirPathLe->clear();

    if (QDir::current().dirName() != "RUN") {
        ui->_flowDirPathLe->setText(QDir::currentPath());
    } else {
        ui->_flowDirPathLe->setText(QFileInfo(QDir::currentPath()).dir().path());
    }
}



// Updates check all stages combobox status
void MainWindow::updateCheckAllCmb()
{
    bool checked = 0;
    bool unchecked = 0;
    for (int i = 0; i < ui->_stagesL->count(); i++) {
        QLayout* layout = qobject_cast<QLayout*>(ui->_stagesL->itemAt(i)->layout());
        if (layout) {
            QCheckBox* cb = qobject_cast<QCheckBox*>(layout->itemAt(1)->widget());
            if (cb) {
                if (cb->isEnabled()) {
                    cb->isChecked() ? checked = 1 : unchecked = 1;
                }
            }
        }
    }

    if (checked && unchecked) {
        ui->_checkStagesCb->setCheckState(Qt::PartiallyChecked);
    } else if (checked) {
         ui->_checkStagesCb->setCheckState(Qt::Checked);
    } else {
         ui->_checkStagesCb->setCheckState(Qt::Unchecked);
    }
}



// Updates force all stages combobox status
void MainWindow::updateForceAllCmb()
{
    bool checked = 0;
    bool unchecked = 0;
    for (int i = 0; i < ui->_stagesL->count(); i++) {
        QLayout* layout = qobject_cast<QLayout*>(ui->_stagesL->itemAt(i)->layout());
        if (layout) {
            QCheckBox* cb = qobject_cast<QCheckBox*>(layout->itemAt(0)->widget());
            if (cb) {
                if (cb->isEnabled()) {
                    cb->isChecked() ? checked = 1 : unchecked = 1;
                }
            }
        }
    }

    if (checked && unchecked) {
        ui->_forceStagesCb->setCheckState(Qt::PartiallyChecked);
    } else if (checked) {
         ui->_forceStagesCb->setCheckState(Qt::Checked);
    } else {
         ui->_forceStagesCb->setCheckState(Qt::Unchecked);
    }
}



// Updates settings button status according config status
void MainWindow::updateSettingsButton(QWidget *w)
{
    StageConfig* config = qobject_cast<StageConfig*>(_updateSettingsButtonMapper.mapping(w));
    QPushButton* settingsPb = qobject_cast<QPushButton*>(w);

    if (config && settingsPb) {
        switch (config->status()) {
        case StageConfig::Status_Empty:
            settingsPb->setIcon(QIcon(":/icons/green_settings"));
            settingsPb->setEnabled(0);
            break;
        case StageConfig::Status_Ok:
            settingsPb->setIcon(QIcon(":/icons/green_settings"));
            break;
        case StageConfig::Status_Warning:
            settingsPb->setIcon(QIcon(":/icons/orange_settings"));
            break;
        case StageConfig::Status_Error:
            settingsPb->setIcon(QIcon(":/icons/red_settings"));
            break;
        }
    }
}
