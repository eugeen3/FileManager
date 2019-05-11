#include "createfile.h"
#include "ui_createfile.h"

CreateFile::CreateFile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateFile)
{
    ui->setupUi(this);
}

CreateFile::~CreateFile()
{
    delete ui;
}

QString CreateFile::on_create_clicked()
{
    this->close();
    return ui->filename->text();
}

void CreateFile::on_cancel_clicked()
{
    this->close();
}
