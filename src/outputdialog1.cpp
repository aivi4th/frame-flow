#include "outputdialog1.h"
#include "ui_outputdialog1.h"
#include <QDebug>
#include <QFile>
#include <QLineEdit>



// Constructor 1
OutputDialog1::OutputDialog1(Flow *flow, QWidget *parent) :
    OutputDialog(flow, parent),
    ui(new Ui::OutputDialog1)
{
    ui->setupUi(this);
    setModal(1);
    setWindowTitle("Выполнение");
    _contentDialog = new TxtFileContentDialog(this);

    foreach(Stage* stage, _flow->stages()) {
        foreach(Operation* operation, stage->operations()) {
            foreach (Step* step, operation->steps()) {

                QHBoxLayout* stepL = new QHBoxLayout;

                // Name
                QLabel* nameL = new QLabel(step->name());
                nameL->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
                nameL->setFixedHeight(25);
                nameL->setStyleSheet("font-size: 10pt");
                stepL->addWidget(nameL);

                // Status
                QLineEdit* statusLe = new QLineEdit;
                statusLe->setFixedSize(130, 25);
                statusLe->setAlignment(Qt::AlignHCenter);
                statusLe->setReadOnly(1);
                stepL->addWidget(statusLe);

                // Result
                QLineEdit* resultLe = new QLineEdit;
                resultLe->setFixedSize(130, 25);
                resultLe->setAlignment(Qt::AlignHCenter);
                resultLe->setReadOnly(1);
                stepL->addWidget(resultLe);

                // Log
                QPushButton* logPb = new QPushButton("Лог");
                logPb->setFixedSize(70, 25);
                stepL->addWidget(logPb);
                _showLogMapper.setMapping(logPb, step);
                connect(logPb, SIGNAL(clicked()), &_showLogMapper, SLOT(map()));

                ui->_stepsL->addLayout(stepL);

                QList<QWidget*> stepWidgets;
                stepWidgets << statusLe;
                stepWidgets << resultLe;
                stepWidgets << logPb;
                _stepsMap[step] = stepWidgets;
            }

            QFrame* line = new QFrame;
            line->setFrameShape(QFrame::HLine);
            line->setFrameShadow(QFrame::Sunken);
            ui->_stepsL->addWidget(line);
        }
    }

    _interruptMb = new QMessageBox(this);
    _interruptMb->setIcon(QMessageBox::Warning);
    _interruptMb->setWindowTitle("Прервывание выполнения");
    _interruptMb->setText("Выполнение не завершено, прервать?");
    _interruptMb->addButton(QMessageBox::Ok);
    _interruptMb->addButton(QMessageBox::Cancel);
    _interruptMb->setDefaultButton(QMessageBox::Cancel);

    connect(ui->_closePb, SIGNAL(clicked()), this, SLOT(reject()));
    connect(_interruptMb->button(QMessageBox::Ok), SIGNAL(clicked()), this, SLOT(interrupt()));
    connect(&_showLogMapper, SIGNAL(mapped(QWidget*)), this, SLOT(showLog(QWidget*)));
    connect(&_refreshTimer, SIGNAL(timeout()), this, SLOT(refresh()));
}



// Destructor
OutputDialog1::~OutputDialog1()
{
    delete ui;
}



// Clears layout
void OutputDialog1::clearLayout(QLayout* layout, bool deleteWidgets)
{
    while (QLayoutItem* item = layout->takeAt(0)) {
        if (deleteWidgets) {
            if (QWidget* widget = item->widget()) {
                delete widget;
            }
        }
        if (QLayout* childLayout = item->layout()) {
            clearLayout(childLayout, deleteWidgets);
        }
        delete item;
    }
}



// Reimplementation of QDialog exec() slot
int OutputDialog1::exec()
{
    _refreshTimer.start(100);
    int width = parentWidget()->width() - 50;
    int height = parentWidget()->height() - 50;
    resize(width, height);
    return QDialog::exec();
}



// Do, when step emits error message
void OutputDialog1::onStepErrorMessage(const QString &message)
{
    Step* step = qobject_cast<Step*>(sender());
    if (step) {
        QLineEdit* statusLe = qobject_cast<QLineEdit*>(_stepsMap[step][0]);
        if (statusLe) {
            statusLe->setToolTip(statusLe->toolTip().append(message));
        }
    }
}



// Do, when step emits output message
void OutputDialog1::onStepOutputMessage(const QString &message)
{
    Q_UNUSED(message);
    /*
    Step* step = qobject_cast<Step*>(sender());
    if (step) {
        QLineEdit* statusLe = qobject_cast<QLineEdit*>(_stepsMap[step][0]);
        if (statusLe) {
            statusLe->setToolTip(statusLe->toolTip().append(message));
        }
    }
    */
}



// Refreshes steps associated widgets
void OutputDialog1::refresh()
{
    foreach(Stage* stage, _flow->stages()) {
        foreach(Operation* operation, stage->operations()) {
            foreach (Step* step, operation->steps()) {

                // Step's status line edit
                QLineEdit* statusLe = qobject_cast<QLineEdit*>(_stepsMap[step][0]);
                if (statusLe) {
                    switch (step->status()) {
                    case Step::Status_UserSkipped:
                        statusLe->setStyleSheet("color: orange");
                        statusLe->setText("Пропущен");
                        break;
                    case Step::Status_Pending:
                        statusLe->setStyleSheet("color: black");
                        statusLe->setText("Ожидание");
                        break;
                    case Step::Status_Launching:
                        statusLe->setStyleSheet("color: orange");
                        statusLe->setText("Запуск");
                        break;
                    case Step::Status_Checking:
                        statusLe->setStyleSheet("color: blue");
                        statusLe->setText("Проверка");
                        break;
                    case Step::Status_AutoSkipped:
                        statusLe->setStyleSheet("color: green");
                        statusLe->setText("Пропущен");
                        break;
                    case Step::Status_Running:
                        statusLe->setStyleSheet("color: blue");
                        statusLe->setText("Выполнение");
                        break;
                   case Step::Status_Complited:
                        statusLe->setStyleSheet("color: green");
                        statusLe->setText("Выполнен");
                        break;
                    case Step::Status_Error:
                        statusLe->setStyleSheet("color: red");
                        statusLe->setText("Ошибка");
                        break;
                    }
                }

                // Step's result line edit
                QLineEdit* resultLe = qobject_cast<QLineEdit*>(_stepsMap[step][1]);
                if (resultLe) {
                    switch (step->result()) {
                    case Step::Result_Unknown:
                        resultLe->setStyleSheet("color: orange");
                        resultLe->setText("Неизвестен");
                        break;
                    case Step::Result_Ok:
                        resultLe->setStyleSheet("color: green");
                        resultLe->setText("Успешно");
                        break;
                    case Step::Result_Warning:
                        resultLe->setStyleSheet("color: orange");
                        resultLe->setText("Предупреждение");
                        break;
                    case Step::Result_Error:
                        resultLe->setStyleSheet("color: red");
                        resultLe->setText("Ошибка");
                        break;
                    }
                }

                // Step's log push button
                QPushButton* logPb = qobject_cast<QPushButton*>(_stepsMap[step][2]);
                if (logPb) {
                    if (QFile::exists(step->logPath())) {
                        logPb->setEnabled(1);
                    } else {
                        logPb->setEnabled(0);
                    }
                }
            }
        }
    }
}



// Reimplementation of QDialog reject() slot
void OutputDialog1::reject()
{
    if (_flow->status() == Flow::Status_Running) {
        _interruptMb->exec();
    } else {
        _flow->reset();
        _refreshTimer.stop();
        QDialog::reject();
    }
}



// Reimplementation of QDialog show() slot
void OutputDialog1::show()
{
    _refreshTimer.start(100);
    //int width = parentWidget()->width() - 50;
    //int height = parentWidget()->height() - 50;
    int width = 960;
    int height = 540;

    resize(width, height);
    return QDialog::show();
}



// Shows step's log content
void OutputDialog1::showLog(QWidget *w)
{
    Step* step = qobject_cast<Step*>(w);
    if (step) {
        _contentDialog->setWindowTitle("Лог выполнения шага \"" + step->name() + "\"");
        _contentDialog->exec(step->logPath());
    }
}






