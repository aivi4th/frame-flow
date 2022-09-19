#ifndef TXTFILECONTENTDIALOG_H
#define TXTFILECONTENTDIALOG_H

#include <QDialog>
#include "contentdialog.h"

namespace Ui {
    class TxtFileContentDialog;
}

class TxtFileContentDialog : public ContentDialog
{
    Q_OBJECT
    
public:
    explicit TxtFileContentDialog(QWidget *parent = 0);
    ~TxtFileContentDialog();

public slots:
    int exec(const QString &filePath);

private:
    Ui::TxtFileContentDialog *ui;
};

#endif // TXTFILECONTENTDIALOG_H
