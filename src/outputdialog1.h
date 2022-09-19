#ifndef OUTPUTDIALOG1_H
#define OUTPUTDIALOG1_H

#include <QList>
#include <QMap>
#include <QMessageBox>
#include <QSignalMapper>
#include <QTimer>
#include "outputdialog.h"
#include "stages/stage.h"
#include "txtfilecontentdialog.h"

namespace Ui {
    class OutputDialog1;
}

class OutputDialog1 : public OutputDialog
{
    Q_OBJECT

public:    
    explicit OutputDialog1(Flow *flow, QWidget *parent = 0);
    ~OutputDialog1();

public slots:
    virtual int exec();
    virtual void reject();
    virtual void show();

private:
    QMessageBox* _interruptMb;
    TxtFileContentDialog *_contentDialog;
    QTimer _refreshTimer;
    QSignalMapper _showLogMapper;
    QMap<Step*, QList<QWidget*> > _stepsMap;
    Ui::OutputDialog1 *ui;

private slots:
    void clearLayout(QLayout* layout, bool deleteWidgets = true);
    void onStepErrorMessage(const QString &message);
    void onStepOutputMessage(const QString &message);
    void refresh();
    void showLog(QWidget* w);
};

#endif // OUTPUTDIALOG1_H
