#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include "calibrestep.h"

#include <QThread>

// Constructor 1
CalibreStep::CalibreStep(const QString &id, const QString &name, const QString &interpretator, const QString &script, const QMap<QString, QString> &args, QWidget *parent) :
    ScriptStep(id, name, interpretator, script, args, parent)
{

}



// Destructor
CalibreStep::~CalibreStep()
{

}



// Returns calibre enviroment variables map
QMap<QString, QString> CalibreStep::calibreVariables(const QString &calibreVersion)
{
    QMap<QString, QString> map;
    QSettings commonConfig(QFileInfo(QApplication::applicationDirPath()).dir().path().append("/cfg/common.conf"), QSettings::IniFormat);
    //args << "/workarea/SANRO/common_cshrc.csh";
    map["LM_LICENSE_FILE"] = commonConfig.value("Calibre/license").toString();
    //map["USE_CALIBRE_VCO"] = "ixl";
    map["CAL_VERSION"] = QString(calibreVersion);
    map["CAD_TOOLS"] = commonConfig.value("cadToolsDir").toString();
    map["CAL_INST_DIR"] = QString(map["CAD_TOOLS"]).append("/mentor/").append(QString(map["CAL_VERSION"]));
    map["MGC_HOME"] = QString(map["CAL_INST_DIR"]);
    map["USE_CALIBRE_64"] = "YES";
    map["MGC_DISABLE_BACKING_STORE"] = "true";
    map["PATH"] = QProcessEnvironment::systemEnvironment().value("PATH").append(":").append(QString(map["MGC_HOME"]).append("/bin"));
    map["CALIBRE_HOME"] = QString(map["MGC_HOME"]);
    return map;
}



// Launches step
void CalibreStep::launch()
{
    setStatus(Status_Launching);
    _permissions = QFile(QString(QDir().homePath()).append("/.cshrc")).permissions();

    if (_process.state() != QProcess::NotRunning) {
        _process.terminate();
        _process.waitForFinished();
    }

    QSettings commonConfig(QFileInfo(QApplication::applicationDirPath()).dir().path().append("/cfg/common.conf"), QSettings::IniFormat);
    QString command = QString();

    command = QString().append(_interpretator).append(" ").append(_script);
    QMap<QString, QString> varMap = calibreVariables(commonConfig.value("Calibre/defaultVersion").toString());
    QProcessEnvironment env(QProcessEnvironment::systemEnvironment());
    foreach(QString var, varMap.keys()) {
        env.insert(var, varMap.value(var));
    }
    _process.setProcessEnvironment(env);

    foreach (QString key, _args.uniqueKeys()) {
        foreach (QString value, _args.values(key)) {
            command.append(" +").append(key).append(" ").append(value);
        }
    }

    Status status = _status;
    _process.start(command);
    if (!_process.waitForStarted()) {
        status = Status_Error;
    }

    setStatus(status);
}



void CalibreStep::launch(const QString &hostName)
{
    setStatus(Status_Launching);

    if (_process.state() != QProcess::NotRunning) {
        _process.terminate();
        _process.waitForFinished();
    }

    QSettings commonConfig(QFileInfo(QApplication::applicationDirPath()).dir().path().append("/cfg/common.conf"), QSettings::IniFormat);
    QString command = QString();

    command.append("ssh ").append(hostName).append(" source ").
                                append(commonConfig.value("Calibre/setVersionScript").toString()).append(" ").
                                append(commonConfig.value("Calibre/defaultVersion").toString()).append(" ").
                                append(commonConfig.value("cadToolsDir").toString()).append(" ").
                                append(commonConfig.value("Calibre/license").toString()).append(" && ").
                                append(_interpretator).append(" ").append(_script);

    if (commonConfig.value("Application/disableUserConfig").toBool()) {
        QFile(QString(QDir().homePath()).append("/.cshrc")).setPermissions(0x0000);
    }

    foreach (QString key, _args.uniqueKeys()) {
        foreach (QString value, _args.values(key)) {
            command.append(" +").append(key).append(" ").append(value);
        }
    }

    Status status = _status;
    _process.start(command);
    if (!_process.waitForStarted()) {
        status = Status_Error;
    }

    setStatus(status);
}


