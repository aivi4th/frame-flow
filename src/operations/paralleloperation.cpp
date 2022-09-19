#include <QApplication>
#include <QDebug>
#include <QSettings>
#include "paralleloperation.h"



// Construct 1
ParallelOperation::ParallelOperation(const QString &id, const QString &name, QMap<QString, QString> args, QWidget *parent) :
    Operation(id, name, args, parent)
{

}



// Destructor
ParallelOperation::~ParallelOperation()
{

}



// Launches stage
void ParallelOperation::launch()
{
    if (QSettings(QApplication::instance()->property("commonConfigPath").toString(), QSettings::IniFormat).value("Application/remoteRun").toBool() && _hostName.isEmpty()) {
        initHostsLoad();
        _hostName = getHostName();
    }
    int parallelRate = QSettings(QApplication::instance()->property("commonConfigPath").toString(), QSettings::IniFormat).value("Application/parallelRate").toInt();
    foreach (Step* step, _steps) {
        if (step->status() == Step::Status_Pending && _runningSteps.length() < parallelRate) {
            _runningSteps.enqueue(step);
        }
    }
    foreach (Step* step, _runningSteps) {
        if (step->status() == Step::Status_Pending) {
            _hostName.isEmpty() ? _runningSteps.dequeue()->launch() : _runningSteps.dequeue()->launch(_hostName);
        }
    }
}



void ParallelOperation::onStepStatusChanged()
{
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
    updateStatus();
}
