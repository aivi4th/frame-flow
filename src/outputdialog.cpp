#include "outputdialog.h"
#include <QAbstractButton>
#include <QDebug>



// Constructor 1
OutputDialog::OutputDialog(Flow* flow, QWidget *parent) :
    QDialog(parent),
    _flow(flow)
{
    connect(_flow, SIGNAL(statusChanged(Flow::Status)), this, SLOT(onFlowStatusChanged(Flow::Status)));
    foreach(Stage* stage, _flow->stages()) {
        connect(stage, SIGNAL(statusChanged(Stage::Status)), this, SLOT(onStageStatusChanged(Stage::Status)));
        foreach(Operation* operation, stage->operations()) {
            connect(operation, SIGNAL(statusChanged(Operation::Status)), this, SLOT(onOperationStatusChanged(Operation::Status)));
            foreach (Step* step, operation->steps()) {
                connect(step, SIGNAL(statusChanged(Step::Status)), this, SLOT(onStepStatusChanged(Step::Status)));
                connect(step, SIGNAL(errorMessage(QString)), this, SLOT(onStepErrorMessage(QString)));
                connect(step, SIGNAL(outputMessage(QString)), this, SLOT(onStepOutputMessage(QString)));
            }
        }
    }
}



// Destructor
OutputDialog::~OutputDialog()
{

}



// Reimplementation of QDialog exec() slot
int OutputDialog::exec()
{
    //int width = parentWidget()->width() - 50;
    //int height = parentWidget()->height() - 50;
    int width = 960;
    int height = 540;
    resize(width, height);
    return QDialog::exec();
}



// Interrupts flow execution and closes output dialog
void OutputDialog::interrupt()
{
    _flow->interrupt();
    reject();
}



// Do, when flow status is changed
void OutputDialog::onFlowStatusChanged(Flow::Status status)
{
    Q_UNUSED(status);
}



// Do, when operation status is changed
void OutputDialog::onOperationStatusChanged(Operation::Status status)
{
    Q_UNUSED(status);
}



// Do, when stage status is changed
void OutputDialog::onStageStatusChanged(Stage::Status status)
{
    Q_UNUSED(status);
}



// Do, when step output message is emitted
void OutputDialog::onStepErrorMessage(const QString &message)
{
    Q_UNUSED(message);
}



// Do, when step output message is emitted
void OutputDialog::onStepOutputMessage(const QString &message)
{
    Q_UNUSED(message);
}



// Do, when step status is changed
void OutputDialog::onStepStatusChanged(Step::Status status)
{
    Q_UNUSED(status);
}



// Reimplementation of QDialog show() slot
void OutputDialog::show()
{
    //int width = parentWidget()->width() - 50;
    //int height = parentWidget()->height() - 50;
    int width = 960;
    int height = 540;
    resize(width, height);
    return QDialog::show();
}


