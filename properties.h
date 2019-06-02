#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QDir>

namespace Ui {
class Properties;
}

class Properties : public QDialog
{
    Q_OBJECT

public:
    explicit Properties(QFileInfo *fileSystemObject = nullptr);
    ~Properties();
    qint64 dirSize(QString dirPath);
    QString formatSize(qint64 size);
    QString dirContains(QString dirPath);

private:
    Ui::Properties *ui;
};

#endif // PROPERTIES_H
