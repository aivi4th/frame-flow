#ifndef OUTPUTDIALOG_H
#define OUTPUTDIALOG_H

#include <QDialog>
#include "flow.h"

class OutputDialog : public QDialog
{
    Q_OBJECT

public:    
    explicit OutputDialog(Flow* flow, QWidget *parent = 0);
    virtual ~OutputDialog();

public slots:
    virtual int exec();
    virtual void interrupt();
    virtual void show();

protected:
    Flow* _flow;

protected slots:
    virtual void onFlowStatusChanged(Flow::Status status);
    virtual void onOperationStatusChanged(Operation::Status status);
    virtual void onStageStatusChanged(Stage::Status status);
    virtual void onStepErrorMessage(const QString &message);
    virtual void onStepOutputMessage(const QString &message);
    virtual void onStepStatusChanged(Step::Status status);
};

#endif // OUTPUTDIALOG_H
