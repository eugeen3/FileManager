#include "copier.h"
#include "mainwindow.h"

Copier::Copier()
{
    copySource = nullptr;
    copyDestination = nullptr;
}

Copier::~Copier() {
    delete thread;
}

bool Copier::copyFile(const QString& from, const QString& to)
{
    MainWindow mw;
    bool success = QFile::copy(from, to);
    if(!success) {
        if(QFile(to).exists()) {
            if(QMessageBox::question(mw.getParentWidget(), "Подтвердите перезапись","Перезаписать существующие фалйлы?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                if(!QFile::remove(to))
                    QMessageBox::critical(mw.getParentWidget(), "Ошибка", "Перезапись файлов не удалась");
                success = QFile::copy(from, to);
            }
        }
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
    }
}

void Copier::paste()
{
    QFileInfo fInfo(copySource);

    qDebug() << "Copy to" << copyDestination;
    if (copySource != nullptr) {
        QFileInfo type(copySource);
        if (type.isDir()) {
            copyDir(copySource, copyDestination);
            copyDestination = nullptr;
        } else if (type.isFile()) {
            QString fileName = fInfo.fileName();
            copyDestination += "/" + fileName;
            copyFile(copySource, copyDestination);
            copyDestination = nullptr;
        }
        emit copyFinished();
    }
}

void Copier::startCopy() {
    thread = new QThread();
    qDebug() << "copy thread started";
    connect(thread, &QThread::started, this, &Copier::paste);
    connect(this, &Copier::copyFinished, thread, &QThread::quit);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
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
