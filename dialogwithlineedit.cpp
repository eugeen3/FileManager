#include "dialogwithlineedit.h"
#include "ui_dialogwithlineedit.h"

DialogWithLineEdit::DialogWithLineEdit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogWithLineEdit)
{
    ui->setupUi(this);
}

DialogWithLineEdit::~DialogWithLineEdit()
{
    delete ui;
}
