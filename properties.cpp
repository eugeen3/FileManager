#include "properties.h"
#include "ui_properties.h"

Properties::Properties(QWidget *parent, QFileInfo *fileSystemObject) :
    QDialog(parent),
    ui(new Ui::Properties)
{
    ui->setupUi(this);


    if (fileSystemObject->isFile()) {
        ui->typeLabel->setText("Тип файла:");
        ui->sizeInfo->setText(formatSize(fileSystemObject->size()) + "("
            + QString::number(fileSystemObject->size()) + " байт)");

        QLabel *createdLabel, *modifiedLabel, *readLabel;
       // QLabel *createdInfo, *modifiedInfo, *readInfo;
        createdLabel = new QLabel("Создан");
        modifiedLabel = new QLabel("Изменён");
        readLabel = new QLabel("Открыт");
        //QDateTime createdTime = fileSystemObject->fileTime();
       // createdInfo = new QLabel(.toString());
    } else if (fileSystemObject->isDir()) {
        ui->typeLabel->setText("Тип:");
        ui->typeInfo->setText("Папка с файлами");
        qint64 directorySize = dirSize(fileSystemObject->canonicalFilePath());
        ui->sizeInfo->setText(formatSize(directorySize) + " ("
            + QString::number(directorySize) + " байт)");

        QLabel *containsLabel, *containsInfo;
        containsLabel = new QLabel("Содержит:");
        containsInfo = new QLabel(dirContains(fileSystemObject->canonicalFilePath()));
        ui->gridLayout->addWidget(containsLabel, 5, 0);
        ui->gridLayout->addWidget(containsInfo, 5, 1);
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
    //TODO
    unsigned int files = 0, dirs = 0;
    QDir dir(dirPath);
    QDir dirF(dirPath);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    files = dir.count();
    dirF.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
    dirs = dirF.count();

    return QString("Файлов: " + QString::number(files)
                   + "; папок :" + QString::number(dirs));
}
