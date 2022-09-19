#include <QApplication>
#include <QtConcurrent>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QSettings>
#include "operation.h"



// Constructor 1
Operation::Operation(const QString &id, const QString &name, QMap<QString, QString> args, QWidget *parent) :
    QWidget(parent),
    _args(args),
    _id(id),
    _name(name)
{
    reset();
}



// Destructor
Operation::~Operation()
{
    qDeleteAll(_steps);
}



// Adds step to operation
void Operation::addStep(Step *step)
{
    if (step) {
        connect(step, SIGNAL(resultChanged(Step::Result)), this, SLOT(onStepResultChanged()));
        connect(step, SIGNAL(statusChanged(Step::Status)), this, SLOT(onStepStatusChanged()));
        _steps << step;

        updateStatus();
        updateResult();
    }
}



// Returns operation args
QMap<QString, QString> Operation::args()
{
    return _args;
}



QString Operation::getHostName()
{
    QString prefferedHost;
    int maxFreeMem = 0;
    QMap<QString,int> hostMap = _getHostsLoadWatcher.result();

    foreach (QString host, hostMap.keys()) {
        if (hostMap.value(host) > maxFreeMem) {
            prefferedHost = host;
            maxFreeMem = hostMap.value(host);
        }
    }

    return prefferedHost;
}


QMap<QString, int> Operation::getHostsLoad()
{
    QMap<QString, int> hostMap;
    QSettings common(QApplication::instance()->property("commonConfigPath").toString(), QSettings::IniFormat);

    QFileDevice::Permissions permissions = QFile(QString(QDir().homePath()).append("/.cshrc")).permissions();
    QFile(QString(QDir().homePath()).append("/.cshrc")).setPermissions(0x0000);

    int maxHostReplyTime = common.value("Application/maxHostReplyTime").toInt();
    foreach (QString hostGroup, common.childGroups()) {
        if (QRegExp("Host_\\d+").exactMatch(hostGroup)) {
            QString host = common.value(hostGroup.append("/name")).toString();
            QProcess p;
            p.start("ssh " + host + " cat /proc/meminfo");
            p.waitForFinished(maxHostReplyTime*1000);
            QRegExp rx("MemFree:\\s+(\\d+)");
            if (rx.indexIn(p.readAllStandardOutput()) > -1) {
                hostMap.insert(host, rx.cap(1).toInt());
            }

        }
    }

    QFile(QString(QDir().homePath()).append("/.cshrc")).setPermissions(permissions);
    return hostMap;
}



void Operation::initHostsLoad()
{
    _getHostsLoadWatcher.cancel();
    _getHostsLoadFuture = QtConcurrent::run(this, &Operation::getHostsLoad);
    _getHostsLoadWatcher.setFuture(_getHostsLoadFuture);
}



// Returns operation id
QString Operation::id()
{
    return _id;
}



// Interrupts Operation execution
void Operation::interrupt()
{
    foreach (Step* step, _steps) {
        step->interrupt();
    }
}



// Returns Operation name
QString Operation::name()
{
    return _name;
}



// Do, when step result is changed
void Operation::onStepResultChanged()
{
    updateResult();
}



// Do, when step status is changed
void Operation::onStepStatusChanged()
{
    updateStatus();
}



// Removes step from operation
void Operation::removeStep(Step *step)
{
    _steps.removeAll(step);
    delete step;
    updateStatus();
    updateResult();
}



// Resets Operation
void Operation::reset()
{
    foreach (Step* step, _steps) {
        step->reset();
    }
    updateStatus();
    updateResult();
}



// Returns Operation result
Operation::Result Operation::result()
{
    return _result;
}



// Sets Operation result
void Operation::setResult(Result result)
{
    if (_result != result) {
        _result = result;
        emit resultChanged(_result);
    }
}



// Sets Operation status
void Operation::setStatus(Status status)
{
    if (_status != status) {

        /*
        switch (status) {
        case Status_Unknown:
            qDebug() << "operation = " << _name << " status = Status_Unknown";
            break;
        case Status_UserSkipped:
            qDebug() << "operation = " << _name << " status = Status_UserSkipped";
            break;
        case Status_Pending:
            qDebug() << "operation = " << _name << " status = Status_Pending";
            break;
        case Status_Running:
            qDebug() << "operation = " << _name << " status = Status_Running";
            break;
        case Status_AutoSkipped:
            qDebug() << "operation = " << _name << " status = Status_AutoSkipped";
            break;
        case Status_Complited:
            qDebug() << "operation = " << _name << " status = Status_Complited";
            break;
        case Status_Error:
            qDebug() << "operation = " << _name << " status = Status_Error";
            break;
        default:
            qDebug() << "operation = " << _name << " status = ???";
            break;
        }
        */

        _status = status;
        emit statusChanged(_status);
    }
}



// Returns Operation status
Operation::Status Operation::status()
{
    return _status;
}


// Returns subOperations
QList<Step*> Operation::steps()
{
    return _steps;
}



// Updates Operation result
void Operation::updateResult()
{
    bool unknown = 0;
    bool ok = 0;
    bool warning = 0;
    bool error = 0;

    foreach (Step* step, _steps) {
        Step::Result stepResult = step->result();
        if (stepResult == Step::Result_Unknown) {
            unknown = 1;
        } else if (stepResult == Step::Result_Ok) {
            ok = 1;
        } else if (stepResult == Step::Result_Warning) {
            warning = 1;
        } else if (stepResult == Step::Result_Error) {
            error = 1;
        }
    }

    if (error) {
        setResult(Result_Error);
    } else if (warning) {
        setResult(Result_Warning);
    } else if (ok) {
        setResult(Result_Ok);
    } else {
        setResult(Result_Unknown);
    }

    Q_UNUSED(unknown);
}



// Updates Operation status
void Operation::updateStatus()
{
    bool userSkipped = false;
    bool pending = false;
    bool launching = false;
    bool checking = false;
    bool autoSkipped = false;
    bool running = false;
    bool complited = false;
    bool error = false;

    foreach (Step* step, _steps) {
        Step::Status stepStatus = step->status();
        if (stepStatus == Step::Status_UserSkipped) {
            userSkipped = true;
        } else if (stepStatus == Step::Status_Pending) {
            pending = true;
        } else if (stepStatus == Step::Status_Launching) {
            launching = true;
        } else if (stepStatus == Step::Status_Checking) {
            checking = true;
        } else if (stepStatus == Step::Status_AutoSkipped) {
            autoSkipped = true;
        } else if (stepStatus == Step::Status_Running) {
            running = true;
        }  else if (stepStatus == Step::Status_Complited) {
            complited = true;
        } else if (stepStatus == Step::Status_Error) {
            error = true;
        }
    }

    if (userSkipped && !pending && !launching && !checking && !running && !autoSkipped && !complited && !error) {
        setStatus(Status_UserSkipped);
    } else if (pending && !launching && !checking && !autoSkipped && !running && !complited && !error) {
        setStatus(Status_Pending);
    } else if (launching || checking || running) {
        setStatus(Status_Running);
    } else if (!pending && !launching && !checking && !running && autoSkipped && !complited && !error) {
        setStatus(Status_AutoSkipped);
    } else if (!pending && !launching && !checking && !running && complited && !error) {
        setStatus(Status_Complited);
    } else if (!launching && !checking && !running && error) {
        setStatus(Status_Error);
    } else {
        setStatus(Status_Unknown);
    }
}


