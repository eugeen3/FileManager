#ifndef CREATEFILE_H
#define CREATEFILE_H

#include <QDialog>

namespace Ui {
class CreateFile;
}

class CreateFile : public QDialog
{
    Q_OBJECT

public:
    explicit CreateFile(QWidget *parent = nullptr);
    ~CreateFile();

private slots:
    QString on_create_clicked();

    void on_cancel_clicked();

private:
    Ui::CreateFile *ui;
};

#endif // CREATEFILE_H
