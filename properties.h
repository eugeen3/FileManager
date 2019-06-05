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
    Properties(QFileInfo *fileSystemObject = nullptr);
    ~Properties();
    qint64 dirSize(QString dirPath);
    QString formatSize(qint64 size);
    void dirContains(QString dirPath);

private:
    Ui::Properties *ui;
    unsigned int files;
    unsigned int dirs;
};

#endif // PROPERTIES_H
