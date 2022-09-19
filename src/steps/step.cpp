#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QMap>
#include <QProcess>
#include "step.h"



// Constructor 1
Step::Step(const QString &id, const QString &name, const QMap<QString, QString> &args, QWidget *parent) :
    QWidget(parent),
    _args(args),
    _id(id),
    _name(name),
    _permissions(QFile(QString(QDir().homePath()).append("/.cshrc")).permissions())
{
    reset();
}



// Destructor
Step::~Step()
{
    QFile(QString(QDir().homePath()).append("/.cshrc")).setPermissions(_permissions);
}



// Returns step args
QMap<QString, QString> Step::args()
{
    return _args;
}



// Returns step id
QString Step::id()
{
    return _id;
}



// Interrupts step execution
void Step::interrupt()
{
    reset();
}



// Returns step log path
QString Step::logPath()
{
    return _logPath;
}



// Returns Step's name
QString Step::name()
{
    return _name;
}



// Resets step
void Step::reset()
{
    setLogPath(QString());
    setResult(Result_Unknown);
    setStatus(Status_Pending);
}



// Returns step result
Step::Result Step::result()
{
    return _result;
}



// Sets step argument with key "key" to value "value"
void Step::setArg(const QString &key, const QString &value)
{
    _args[key] = value;
}



// Sets step log path
void Step::setLogPath(const QString &logPath)
{
    if (_logPath != logPath) {
        _logPath = logPath;
        emit logPathChanged(_logPath);
    }
}



// Sets step result
void Step::setResult(Result result)
{
    if (_result != result) {
        _result = result;
        emit resultChanged(_result);
    }
}



// Sets step status
void Step::setStatus(Status status)
{
    if (_status != status) {

        /*
        switch (status) {
        case Status_UserSkipped:
            qDebug() << "step = " << _name << " status = Status_UserSkipped";
            break;
        case Status_Pending:
            qDebug() << "step = " << _name << " status = Status_Pending";
            break;
        case Status_Launching:
            qDebug() << "step = " << _name << " status = Status_Launching";
            break;
        case Status_Checking:
            qDebug() << "step = " << _name << " status = Status_Checking";
            break;
        case Status_AutoSkipped:
            qDebug() << "step = " << _name << " status = Status_AutoSkipped";
            break;
        case Status_Running:
            qDebug() << "step = " << _name << " status = Status_Running";
            break;
        case Status_Complited:
            qDebug() << "step = " << _name << " status = Status_Complited";
            break;
        case Status_Error:
            qDebug() << "step = " << _name << " status = Status_Error";
            break;
        default:
            qDebug() << "step = " << _name << " status = ???";
            break;
        }
        */

        _status = status;
        emit statusChanged(_status);
    }
}



// Returns step status
Step::Status Step::status()
{
    return _status;
}


