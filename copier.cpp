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

void Copier::copyFile(const QString& from, const QString& to)
{
    bool success = QFile::copy(from, to);
    if(!success) {
        if(QFile(to).exists()) {
            QFile::remove(to);
            QFile::copy(from, to);
        }
        /*if(QMessageBox::question(this, "Подтвердите перезапись","Перезаписать существующие фалйлы?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                if(!QFile::remove(to))
                    QMessageBox::critical(this, "Ошибка", "Перезапись файлов не удалась");
                success = QFile::copy(from, to);
            }*/
    }
}


void Copier::copyDir(const QString &src, const QString &dst)
{
    QDir dir(src);
    if (! dir.exists())
        return;

    foreach (QString curDir, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString dst_path = dst + QDir::separator() + curDir;
        dir.mkpath(dst_path);
        copyDir(src+ QDir::separator() + curDir, dst_path);
    }

    foreach (QString curFile, dir.entryList(QDir::Files)) {
        Copier::copyFile(src + QDir::separator() + curFile, dst + QDir::separator() + curFile);
    }
}

void Copier::paste()
{
    QFileInfo fInfo(copySource);
    qDebug() << "Copy to" << copyDestination;
    if (copySource != nullptr) {
        QFileInfo type(copySource);
        if (type.isDir()) {
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
            newDir.remove(newDir.absolutePath() + oldDir.dirName());
            copyDestination = nullptr;
        }
        else if (type.isFile()) {
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
