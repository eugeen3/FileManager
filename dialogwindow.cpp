#include "dialogwindow.h"
#include "ui_dialogwindow.h"

DialogWindow::DialogWindow(QWidget *parent, QString msg, bool addLineEdit) :
    QDialog(parent),
    ui(new Ui::DialogWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    ui->message->setText(msg);
    if(addLineEdit == true) {
        name = new QLineEdit();
        if(name != nullptr) ui->mainLayout->addWidget(name, 0, 1, 1, 2);
    }
}

DialogWindow::~DialogWindow()
{
    delete ui;
}

QString DialogWindow::getLineEditText() {
    if (name != nullptr) {
        return name->text();
    }
}

void DialogWindow::setText(QString text) {
    ui->message->setText(text);
}


void DialogWindow::on_cancelButton_clicked()
{
    this->close();
}
