#include "properties.h"
#include "ui_properties.h"

Properties::Properties(QFileInfo *fileSystemObject) :
    ui(new Ui::Properties)
{
    ui->setupUi(this);

    files = 0;
    dirs = 0;

    if (fileSystemObject->isFile()) {
        ui->typeInfo->setText('.' + fileSystemObject->fileName().section(".", -1) + " файл");

        ui->sizeInfo->setText(formatSize(fileSystemObject->size()) + "("
                              + QString::number(fileSystemObject->size()) + " байт)");

        /*QLabel *createdLabel, *modifiedLabel, *readLabel;
        QLabel *createdInfo, *modifiedInfo, *readInfo;
        createdLabel = new QLabel("Создан");
        modifiedLabel = new QLabel("Изменён");
        readLabel = new QLabel("Открыт");
        QDateTime createdTime = fileSystemObject->fileTime();
        createdInfo = new QLabel(.toString());
        */
    } else if (fileSystemObject->isDir()) {
        ui->typeInfo->setText("Папка с файлами");

        qint64 directorySize = dirSize(fileSystemObject->canonicalFilePath());
        ui->sizeInfo->setText(formatSize(directorySize) + " ("
                              + QString::number(directorySize) + " байт)");

        QLabel *containsLabel, *containsInfo;
        containsLabel = new QLabel("Содержит:");
        dirContains(fileSystemObject->canonicalFilePath());
        containsInfo = new QLabel("Файлов: " + QString::number(files) + "; папок :" + QString::number(dirs));
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

    for(QString filePath : dir.entryList(QDir::Files|QDir::System|QDir::Hidden)) {
        QFileInfo fi(dir, filePath);
        size += fi.size();
    }

    for(QString childDirPath : dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot|QDir::System|QDir::Hidden)) {
        size += dirSize(dirPath + QDir::separator() + childDirPath);
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

void Properties::dirContains(QString dirPath) {
    QDir dir(dirPath), dirF(dirPath);
    dirF.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    files += dirF.count();
    dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
    dirs += dir.count();

    for(QString childDirPath : dir.entryList()) {
        dirContains(dirPath + QDir::separator() + childDirPath);
    }
}
