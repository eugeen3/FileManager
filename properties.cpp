#include "properties.h"
#include "ui_properties.h"

Properties::Properties(QWidget *parent, QFileInfo *fileSystemObject) :
    QDialog(parent),
    ui(new Ui::Properties)
{
    ui->setupUi(this);


    if (fileSystemObject->isFile()) {
        ui->typeLabel->setText("Тип файла:");
        ui->sizeInfo->setText(formatSize(fileSystemObject->size()) + "(" + QString::number(fileSystemObject->size()) + " байт)");
    } else if (fileSystemObject->isDir()) {
        ui->typeLabel->setText("Тип:");
        ui->typeInfo->setText("Папка с файлами");
        qint64 directorySize = dirSize(fileSystemObject->absolutePath());
        ui->sizeInfo->setText(formatSize(directorySize) + "(" + QString::number(directorySize) + " байт)");
    } else {
        QMessageBox::critical(this, "FileManager", "Не удалось перейти по указаному пути");
    }

    QWidget::setWindowTitle("Свойства: " + fileSystemObject->fileName());
    ui->pathInfo->setText(fileSystemObject->canonicalPath());
}

Properties::~Properties()
{
    delete ui;
}

qint64 Properties::dirSize(QString dirPath) {
    qint64 size = 0;
    QDir dir(dirPath);

    QDir::Filters fileFilters = QDir::Files|QDir::System|QDir::Hidden;
    for(QString filePath : dir.entryList(fileFilters)) {
        QFileInfo fi(dir, filePath);
        size+= fi.size();
    }

    QDir::Filters dirFilters = QDir::Dirs|QDir::NoDotAndDotDot|QDir::System|QDir::Hidden;
    for(QString childDirPath : dir.entryList(dirFilters)) {
        size+= dirSize(dirPath + QDir::separator() + childDirPath);
    }

    return size;
}

QString Properties::formatSize(qint64 size) {
    QStringList units = {"байт", "КБ", "МБ", "ГБ", "ТБ", "ПБ"};
    int i;
    double outputSize = size;
    for(i = 0; i < units.size() - 1; i++) {
        if(outputSize < 1024) break;
        outputSize = outputSize/1024;
    }
    return QString("%0 %1").arg(outputSize, 0, 'f', 2).arg(units[i]);
}

QString Properties::dirContains(QString dirPath) {
    int files = 0, dirs = 0;
    QDir dir(dirPath);

    QDir::Filters fileFilters = QDir::Files|QDir::System|QDir::Hidden;
    for(QString filePath : dir.entryList(fileFilters)) {
        QFileInfo fi(dir, filePath);
        size+= fi.size();
    }

    QDir::Filters dirFilters = QDir::Dirs|QDir::NoDotAndDotDot|QDir::System|QDir::Hidden;
    for(QString childDirPath : dir.entryList(dirFilters)) {
        size+= dirSize(dirPath + QDir::separator() + childDirPath);
    }

    return QString("%0 %1").arg(outputSize, 0, 'f', 2).arg(units[i]);
}
