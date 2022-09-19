#include "authenticationdialog.h"
#include "ui_authenticationdialog.h"


// Конструктор
AuthenticationDialog::AuthenticationDialog(QWidget *parent, const QString &user, const QString &password) :
    QDialog(parent),
    ui(new Ui::AuthenticationDialog)
{
    ui->setupUi(this);
    if (!user.isEmpty()) {
        ui->_usernameLe->setText(user);
    } else {
        ui->_usernameLe->setText(qgetenv("USER"));
    }
    ui->_passwordLe->setText(password);
    if (ui->_usernameLe->text().isEmpty()) {
        ui->_usernameLe->setFocus();
    } else if (ui->_passwordLe->text().isEmpty()) {
        ui->_passwordLe->setFocus();
    } else {
        ui->_connectPb->setFocus();
    }
    connect(ui->_connectPb, SIGNAL(clicked(bool)), this, SLOT(accept()));
}


// Деструктор
AuthenticationDialog::~AuthenticationDialog()
{
    delete ui;
}


// Возвращает введенный пароль
QString AuthenticationDialog::password() const
{
    return ui->_passwordLe->text();
}


// Возвращает true, если отмечен флаг "Запомнить", false в противном случае
bool AuthenticationDialog::rememberIsChecked() const
{
    return ui->_rememberCb->isChecked();
}


// Возвращает введенное имя пользователя
QString AuthenticationDialog::username() const
{
    return ui->_usernameLe->text();
}
