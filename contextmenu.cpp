#include "contextmenu.h"

ContextMenu::ContextMenu()
{
    QMenu * contextMenu = new QMenu();
    bool isFolder;

    QFileInfo currentPos(getPathByCurrentModelIndex());
    if(currentPos.isDir()) isFolder = true;
    else isFolder = false;

    QAction *openFileSystem = new QAction("Открыть");
    QAction *renameFileSystem = new QAction("Переименовать");
    QAction *copyFileSystem = new QAction("Копировать");
    QAction *cutFileSystem = new QAction("Вырезать");
    QAction *deleteFileSystem = new QAction("Удалить");
    QAction *propertiesFileSystem = new QAction("Свойства");
    QAction *pasteFileSystem;

    if (isFolder) {
        pasteFileSystem = new QAction("Вставить");
        if (copySource == nullptr) {
            pasteFileSystem->setEnabled(false);
        }
    }

    contextMenu->addAction(openFileSystem);
    contextMenu->addSeparator();

    contextMenu->addAction(copyFileSystem);
    contextMenu->addAction(cutFileSystem);

    if (isFolder) {
        contextMenu->addAction(pasteFileSystem);
        connect(pasteFileSystem, &QAction::triggered, this, &MainWindow::paste);
    }

    contextMenu->addSeparator();

    contextMenu->addAction(renameFileSystem);
    contextMenu->addAction(deleteFileSystem);
    contextMenu->addSeparator();

    contextMenu->addAction(propertiesFileSystem);

    connect(openFileSystem, &QAction::triggered, this, &MainWindow::fileSystemGoForward);
    connect(copyFileSystem, &QAction::triggered, this, &ContextMenu::copy);
    //connect(cutFileSystem, &QAction::triggered, this, &MainWindow::cut);
    connect(renameFileSystem, &QAction::triggered, this, &ContextMenu::rename);
    connect(deleteFileSystem, &QAction::triggered, this, &ContextMenu::removeKebab);
    connect(propertiesFileSystem, &QAction::triggered, this, &ContextMenu::showProperties);

    /* Вызываем контекстное меню */
    contextMenu->popup(filesCurrentView->viewport()->mapToGlobal(pos));
}


void ContextMenu::removeKebab() {
    QString sPath = getPathByCurrentModelIndex();
    QFileInfo fPath = sPath;

    if(fPath.isDir()) {
        QDir dir(sPath);
        if(!dir.removeRecursively()) {
            QMessageBox::critical("Удаление", "Не удалось удалить указанную папку");
        }
    } else if(fPath.isFile()) {
        QFile file;
        if(!file.remove(sPath)) {
            QMessageBox::critical(this, "Удаление", "Не удалось удалить указанный файл");
        }
    } else {
        QMessageBox::critical(this, "FileManager", "Не удалось перейти по указаному пути");
    }
}

void ContextMenu::copy() {
    copySource = getPathByCurrentModelIndex();
    qDebug() << "Copy from" << copySource;
}

bool ContextMenu::copyFile(const QString& from, const QString& to)
{
    bool success = QFile::copy(from, to);
    if(!success) {
        if(QFile(to).exists()) {
            if(QMessageBox::question(this, tr("Подтвердите перезапись"), tr("Перезаписать существующие фалйлы?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                if(!QFile::remove(to))
                    QMessageBox::critical(this, tr("Ошибка"), tr("Перезапись файлов не удалась"));
                success = QFile::copy(from, to);
            }
        }
    }
    return success;
}

void ContextMenu::copyDir(const QString &src, const QString &dst)
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

void ContextMenu::paste() {
    copyDestination = getPathByCurrentModelIndex();
    QFileInfo fInfo(copySource);

    qDebug() << "Copy to" << copyDestination;
    if (copySource != nullptr) {
        QFileInfo type(copySource);
        if (type.isDir()) {
            copyDir(copySource, copyDestination);
            copyDestination = nullptr;
        } else if(type.isFile()) {
            QString fileName = fInfo.fileName();
            copyDestination += "/" + fileName;
            copyFile(copySource, copyDestination);
            copyDestination = nullptr;
        } else {

        }
    }
}

void ContextMenu::rename() {
    /*
    QString sPath = getPathByCurrentModelIndex();
    qDebug() << "ABS PATH" << sPath;
    QFileInfo fPath = sPath;
    QString newName, oldName = sPath;

    DialogWindow dialogWindow(,"Введите имя файла", true);
    dialogWindow.setModal(true);
    dialogWindow.exec();

    if (fPath.isDir()) {
        QDir directory;
        QString rawFileName = dialogWindow.getLineEditText();
        newName = sPath.section("/", 0, -2) + "/" + rawFileName;

        while(!directory.rename(oldName, newName)) {
            dialogWindow.setText("Ошибка переименования");

        }
    }
    else if (fPath.isFile()) {
        QFile file;
        QString rawFileName = "name312312312.txt";

        newName = sPath.section("/", 0, -2) + "/" + rawFileName;
        if(!file.rename(oldName, newName)) {
            QMessageBox::critical(this, "Rename", "Rename error");
        }
    } else {
        QMessageBox::critical(this, "FileManager", "Не удалось перейти по указаному пути");
    }
    */
}

void ContextMenu::showProperties() {
    QThread *props = new QThread();
    Properties *propWin = new Properties(new QFileInfo(getPathByCurrentModelIndex()));
    propWin->moveToThread(props);
    propWin->exec();
}
