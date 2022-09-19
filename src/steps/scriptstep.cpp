#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include "scriptstep.h"

#include <QThread>

// Constructor 1
ScriptStep::ScriptStep(const QString &id, const QString &name, const QString &interpretator, const QString &script, const QMap<QString, QString> &args, QWidget *parent) :
    Step(id, name, args, parent),
    _interpretator(interpretator),
    _script(script)
{
    connect(&_process, SIGNAL(finished(int)), this, SLOT(onProcessFinished(int)));
    connect(&_process, SIGNAL(readyReadStandardError()), this, SLOT(onProcessReadyReadStandardError()));
    connect(&_process, SIGNAL(readyReadStandardOutput()), this, SLOT(onProcessReadyReadStandardOutput()));
}



// Destructor
ScriptStep::~ScriptStep()
{
    if (_process.state() != QProcess::NotRunning) {
        _process.terminate();
        _process.waitForFinished();
    }
}



// Interrupts script execution
void ScriptStep::interrupt()
{
    if (_process.state() != QProcess::NotRunning) {
        _process.terminate();
        _process.waitForFinished();
    }
    Step::interrupt();
}



// Launches step
void ScriptStep::launch()
{
    setStatus(Status_Launching);
    _permissions = QFile(QString(QDir().homePath()).append("/.cshrc")).permissions();

    if (_process.state() != QProcess::NotRunning) {
        _process.terminate();
        _process.waitForFinished();
    }

    QString command = QString();

    command.append(_interpretator).append(" ").append(_script);

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



void ScriptStep::launch(const QString &hostName)
{
    setStatus(Status_Launching);
    _permissions = QFile(QString(QDir().homePath()).append("/.cshrc")).permissions();

    if (_process.state() != QProcess::NotRunning) {
        _process.terminate();
        _process.waitForFinished();
    }

    QSettings commonConfig(QFileInfo(QApplication::applicationDirPath()).dir().path().append("/cfg/common.conf"), QSettings::IniFormat);
    QString command = QString();

    command.append("ssh ").append(hostName).append(" ").append(_interpretator).append(" ").append(_script);
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



// Do, when process is finished
void ScriptStep::onProcessFinished(const int &exitCode)
{
    Result result = _result;
    Status status = _status;
    if (exitCode != 0) {
        result = Result_Unknown;
        status = Status_Error;
    }
    setResult(result);
    setStatus(status);
}



// Do, when process emits error message
void ScriptStep::onProcessReadyReadStandardError()
{
    emit errorMessage(_process.readAllStandardError());
}



// Do, when process emits output message
void ScriptStep::onProcessReadyReadStandardOutput()
{
    QStringList outputLines;
    foreach (QString line, _process.readAllStandardOutput().split('\n')) {
        if (QRegExp("^\\s*\\[signal\\][\\s\\S]*$").exactMatch(line)) {
            if (QRegExp("^\\s*\\[signal\\]\\s*status\\s*=\\s*\\S+\\s*$").exactMatch(line)) {
                Status status = _status;
                QString status1 = line.split('=')[1].trimmed();
                if (status1 == "pending") {
                    status = Status_Pending;
                } else if (status1 == "launching") {
                    status = Status_Launching;
                } else if (status1 == "checking") {
                    QFile(QString(QDir().homePath()).append("/.cshrc")).setPermissions(_permissions);
                    status = Status_Checking;
                } else if (status1 == "running") {
                    status = Status_Running;
                } else if (status1 == "skipped") {
                    status = Status_AutoSkipped;
                } else if (status1 == "complited") {
                    status = Status_Complited;
                } else if (status1 == "error") {
                    status = Status_Error;
                }
                setStatus(status);
            } else if (QRegExp("^\\s*\\[signal\\]\\s*result\\s*=\\s*\\S+\\s*$").exactMatch(line)) {
                Result result = _result;
                QString result1 = line.split('=')[1].trimmed();
                if (result1 == "unknown") {
                    result = Result_Unknown;
                } else if (result1 == "ok") {
                    result = Result_Ok;
                } else if (result1 == "warning") {
                    result = Result_Warning;
                } else if (result1 == "error") {
                    result = Result_Error;
                }
                setResult(result);
            } else if (QRegExp("^\\s*\\[signal\\]\\s*log\\s*=\\s*\\S+\\s*$").exactMatch(line)) {
                setLogPath(line.split('=')[1].trimmed());
            }
        } else {
            outputLines << line;
        }
    }

    if (!outputLines.isEmpty()) {
        emit outputMessage(outputLines.join("\n"));
    }
}
