#include <QApplication>
#include <QDebug>
#include <QSettings>
#include "serialoperation.h"



// Construct 1
SerialOperation::SerialOperation(const QString &id, const QString &name, QMap<QString, QString> args, QWidget *parent) :
    Operation(id, name, args, parent)
{

}



// Destructor
SerialOperation::~SerialOperation()
{

}



// Launches stage
void SerialOperation::launch()
{
    if (QSettings(QApplication::instance()->property("commonConfigPath").toString(), QSettings::IniFormat).value("Application/remoteRun").toBool() && _hostName.isEmpty()) {
        initHostsLoad();
        _hostName = getHostName();
    }
    foreach (Step* step, _steps) {
        if (step->status() == Step::Status_Pending) {
            _hostName.isEmpty() ? step->launch() : step->launch(_hostName);
            break;
        }
    }
}



// Do, when step status is changed
void SerialOperation::onStepStatusChanged()
{
    if (_status == Status_Running) {
        Step* prevStep = qobject_cast<Step*>(sender());
        if (prevStep) {
            Step::Status prevStatus = prevStep->status();
            Step::Result prevResult = prevStep->result();
            if ((prevStatus == Step::Status_Complited || prevStatus == Step::Status_AutoSkipped) && prevResult != Step::Result_Error && prevResult != Step::Result_Unknown && prevStep != _steps.last()) {
                int prevIndex = _steps.indexOf(prevStep);
                if (prevIndex != -1) {
                    Step* nextStep;
                    for (int nextIndex = prevIndex + 1; nextIndex < _steps.size(); nextIndex++) {
                        nextStep = _steps[nextIndex];
                        if (nextStep->status() == Step::Status_Pending) {
                            _hostName.isEmpty() ? nextStep->launch() : nextStep->launch(_hostName);
                            break;
                        }
                    }
                }
            }
        }
    }
    updateStatus();
}


