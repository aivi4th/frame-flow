#include <QDebug>
#include <QFile>
#include <QTextStream>
#include "txtfilecontentdialog.h"
#include "ui_txtfilecontentdialog.h"



// Constructor 1
TxtFileContentDialog::TxtFileContentDialog(QWidget *parent) :
    ContentDialog(parent),
    ui(new Ui::TxtFileContentDialog)
{
    ui->setupUi(this);
}



// Destructor
TxtFileContentDialog::~TxtFileContentDialog()
{
    delete ui;
}



// Reimplemetation of QDialog exec() slot
int TxtFileContentDialog::exec(const QString &filePath)
{
    ui->_contentTe->clear();

    QFile file(filePath);
    if (QFile::exists(filePath)) {
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream in(&file);
            ui->_contentTe->insertHtml(QString("<pre>").append(in.readAll().replace(QRegExp("ERROR"), "<font color=red>ERROR</font>").replace(QRegExp("WARNING"), "<font color=orange>WARNING</font>")).append("</pre>"));
            file.close();
        }
    }

    int width = parentWidget()->width() - 50;
    int height = parentWidget()->height() - 50;
    resize(width, height);

    return QDialog::exec();
}
