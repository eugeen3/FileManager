#include "copier.h"
#include "mainwindow.h"

Copier::Copier()
{
    copySource = nullptr;
    copyDestination = nullptr;
    thread = new QThread();
}

Copier::~Copier() {
    delete thread;
}

void Copier::copyFile(const QString& from, const QString& to)
{
    bool success = QFile::copy(from, to);
    if(!success) {
        if(QFile(to).exists()) {
            QFile::remove(to);
            QFile::copy(from, to);
        }
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
        if (QFile(dst + QDir::separator() + curFile).exists()) {
            QFile::remove(dst + QDir::separator() + curFile);
        }
        Copier::copyFile(src + QDir::separator() + curFile, dst + QDir::separator() + curFile);
    }
}

void Copier::paste()
{
    QFileInfo fInfo(copySource);
    if (copySource != nullptr) {
        QFileInfo type(copySource);
        if (type.isDir()) {
            QDir oldDir(copySource), newDir(copyDestination);
            newDir.setPath(copyDestination + QDir::separator() + oldDir.dirName());
            newDir.mkpath(oldDir.dirName());
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
    connect(thread, &QThread::started, this, &Copier::paste);
    connect(this, &Copier::copyFinished, thread, &QThread::quit);
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
