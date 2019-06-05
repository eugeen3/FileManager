#include "copier.h"
#include "mainwindow.h"

Copier::Copier()
{
    copySource = nullptr;
    copyDestination = nullptr;
    thread = new QThread();
}

Copier::~Copier() {
   // delete thread;
}

bool Copier::copyFile(const QString& from, const QString& to)
{
    bool success = QFile::copy(from, to);
    if(!success) {
        /*if(QFile(to).exists()) {
            if(QMessageBox::question(MainWindow::getParentWidget(), "Подтвердите перезапись","Перезаписать существующие фалйлы?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                if(!QFile::remove(to))
                    QMessageBox::critical(MainWindow::getParentWidget(), "Ошибка", "Перезапись файлов не удалась");*/
                success = QFile::copy(from, to);
            }
    return success;
    }



void Copier::copyDir(const QString &src, const QString &dst)
{
    QDir dir(src);
    if (! dir.exists())
        return;

    foreach (QString d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString dst_path = dst + QDir::separator() + d;
        dir.mkpath(dst_path);
        copyDir(src+ QDir::separator() + d, dst_path);
    }

    foreach (QString f, dir.entryList(QDir::Files)) {
        QFile::copy(src + QDir::separator() + f, dst + QDir::separator() + f);
       // MainWindow::setProgressBarCur(dirSize(copyDestination) / INT_MAX);
    }
}

void Copier::paste()
{
    QFileInfo fInfo(copySource);
    qDebug() << "Copy to" << copyDestination;
    if (copySource != nullptr) {
        QFileInfo type(copySource);
        if (type.isDir()) {
           // MainWindow::setProgressBarMin(0);
          //  MainWindow::setProgressBarMax(dirSize(copySource)/ INT_MAX);
            QDir oldDir(copySource), newDir(copyDestination), tempDir(copyDestination);
            //tempDir.setPath(copyDestination + QDir::separator() + oldDir.dirName());
            newDir.setPath(copyDestination + QDir::separator() + oldDir.dirName());
            /*if (newDir.exists()) {
                if(QMessageBox::question(MainWindow::getParentWidget(), "Подтвердите перезапись","Перезаписать существующие фалйлы?",
                                         QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                    if(!QFile::remove(newDir.absolutePath()))
                        QMessageBox::critical(MainWindow::getParentWidget(), "Ошибка", "Перезапись файлов не удалась");
                }
            }*/
           // else
            newDir.mkpath(oldDir.dirName());
            qDebug() << "newDir" << newDir.absolutePath();
            copyDir(copySource, newDir.absolutePath());
            copyDestination = nullptr;
        }
        else if (type.isFile()) {
           // MainWindow::setProgressBarMin(0);
           // MainWindow::setProgressBarMax(type.size() / INT_MAX);
            QString fileName = fInfo.fileName();
            copyDestination += QDir::separator() + fileName;
            copyFile(copySource, copyDestination);
            copyDestination = nullptr;
        }
        emit copyFinished();
    }
}

void Copier::startCopy() {
    this->moveToThread(thread);
    qDebug() << "copy thread started";
    connect(thread, &QThread::started, this, &Copier::paste);
    connect(this, &Copier::copyFinished, thread, &QThread::quit);
    connect(this, &Copier::copyFinished, this, &Copier::changeState);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    isFinished = false;
    thread->start();
}

void Copier::setSourcePath(const QString &src) {
    copySource = src;
}

void Copier::setDestinationPath(const QString &dst) {
    copyDestination = dst;
}

QString Copier::getSourcePath() {
    return copySource;
}

QString Copier::getDestinationPath() {
    return copyDestination;
}

void Copier::changeState() {
    qDebug() << "Copy is finished";
    isFinished = true;
}
