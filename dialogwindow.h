#ifndef DIALOGWINDOW_H
#define DIALOGWINDOW_H

#include <QDialog>
#include <QLineEdit>

namespace Ui {
class DialogWindow;
}

class DialogWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DialogWindow(QWidget *parent = nullptr, QString msg = nullptr, bool addLineEdit = false);
    ~DialogWindow();

private:
    Ui::DialogWindow *ui;
    QLineEdit *name;
};

#endif // DIALOGWINDOW_H