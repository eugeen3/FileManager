#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString rootPath = QDir::rootPath();
    //QString rootPathLabel = "Компьютер";

    initFiles();
    initDirs();
    initFilesList();
    initFilesTable();
    initTree();

    dirPath = new QCompleter(files);
    ui->goToPath->setCompleter(dirPath);
    ui->goToPath->setText(rootPathLabel);
    filesCurrentView = filesTable;
    ui->rightLayout->addWidget(filesCurrentView, 2, 0, 1, 3);
    ui->splitter->setStretchFactor(1, 1);

    QLineEdit *lb = new QLineEdit("TEXT");
    QProgressBar *prBar = new QProgressBar(this);
    ui->statusBar->addWidget(lb);
    ui->statusBar->addWidget(prBar);

    connect(filesCurrentView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomMenuRequested(QPoint)));
    connect(filesCurrentView, &QAbstractItemView::doubleClicked, this, &MainWindow::fileSystemGoForward);
    //connect(filesCurrentView, &QLineEdit::returnPressed, this, &MainWindow::fileSystemGoForward);
    connect(ui->goBack, &QPushButton::clicked, this, &MainWindow::dirUp);
    connect(ui->toTheRoot, &QPushButton::clicked, this, &MainWindow::dirRoot);

    //connect(filesCurrentView, &MainWindow::enterPressed, this, &MainWindow::fileSystemGoForward);
    //connect(filesCurrentView, &MainWindow::deletePressed, this, &MainWindow::removeKebab);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initFiles() {
    files = new QFileSystemModel(this);
    files->setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    files->setRootPath(rootPath);
}

void MainWindow::initDirs() {
    directories = new QFileSystemModel(this);
    directories->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    directories->setRootPath(rootPath);
}

void MainWindow::initFilesList() {
    filesList = new QListView();
    filesList->setModel(files);
    filesList->setContextMenuPolicy(Qt::CustomContextMenu);
}

void MainWindow::initFilesTable() {
    filesTable = new QTableView();
    filesTable->setModel(files);
    filesTable->verticalHeader()->hide();
    filesTable->setSortingEnabled(true);
    filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    filesTable->setSelectionMode(QAbstractItemView::SingleSelection);
    filesTable->setContextMenuPolicy(Qt::CustomContextMenu);
}

void MainWindow::initTree() {
    ui->treeView->setModel(directories);
    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(3);
    ui->treeView->header()->hide();
}

void MainWindow::resetModelIndex(QModelIndex &index) {
    //index = -1;
}

/*
void MainWindow::keyPressEvent(QKeyEvent *keyEvent) {
    if (index.isValid()) {
        switch (keyEvent->key()) {
            case Qt::Key_Enter : emit enterPressed(index);
            case Qt::Key_Delete : emit deletePressed(index);
        }
    }
}
*/

void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    QString path = directories->fileInfo(index).absoluteFilePath();
    filesCurrentView->setRootIndex(files->setRootPath(path));
    ui->goToPath->setText(path);
}

void MainWindow::fileSystemGoForward()
{
    const QModelIndex index = getCurrentModelIndex();
    QString sPath = files->fileInfo(index).absoluteFilePath();
    QTextCodec *codec_1251 = QTextCodec::codecForName("Windows-1251");
    QByteArray baPath = sPath.toUtf8();
    QString string = codec_1251->toUnicode(baPath);
    qDebug() << "BAPATH" << baPath;

    if(files->fileInfo(index).isDir()) {
        qDebug() << "STR" << sPath;
        filesCurrentView->setRootIndex(files->setRootPath(sPath));
        ui->goToPath->setText(sPath);
    }
    else if (files->fileInfo(index).isFile()) {
        getValidPath(sPath);
        //Fix spaces and cyrillic symbols in path

        QString program;
        QString initCmd;
#ifdef __linux__

#elif _WIN32
        program = "C:\\Windows\\System32\\cmd.exe";
        initCmd = "cmd /C";
        sPath.replace("/", "\\");
#endif
        initCmd += sPath;
        process = new QProcess();
        QStringList args;
        args.append(initCmd);
        qDebug() << "Open File" <<sPath;
        process->setProgram(program);
        process->setArguments(args);
        process->startDetached();
    }
}

void MainWindow::on_list_triggered()
{
    if (filesCurrentView != filesList) {
        disconnect(filesCurrentView, &QAbstractItemView::doubleClicked, this, &MainWindow::fileSystemGoForward);
        cleanLayout(ui->rightLayout);
        if (filesTable == nullptr) initFilesList();
        filesCurrentView = filesList;
        ui->rightLayout->addWidget(filesList, 2, 0, 1, 3);
        connect(filesCurrentView, &QAbstractItemView::doubleClicked, this, &MainWindow::fileSystemGoForward);
    }
}

void MainWindow::on_table_triggered()
{
    if (filesCurrentView != filesTable) {
        disconnect(filesCurrentView, &QAbstractItemView::doubleClicked, this, &MainWindow::fileSystemGoForward);
        cleanLayout(ui->rightLayout);
        if (filesTable == nullptr) initFilesTable();
        filesCurrentView = filesTable;
        ui->rightLayout->addWidget(filesTable, 2, 0, 1, 3);
        connect(filesCurrentView, &QAbstractItemView::doubleClicked, this, &MainWindow::fileSystemGoForward);
    }
}

void MainWindow::cleanLayout(QLayout *layout) {
    if (layout) {
        if(layout->count() > 3){
            QLayoutItem *item = layout->takeAt(3);
            QWidget* widget = item->widget();
            if(widget)
                delete widget;
            delete item;
        }
    }
}

void MainWindow::on_goToPath_returnPressed()
{
    QString sPath = ui->goToPath->text();
    QDir dPath = ui->goToPath->text();
    QFileInfo fPath = ui->goToPath->text();

    QTextCodec *codec_1251 = QTextCodec::codecForName("Windows-1251");
    QByteArray baPath = sPath.toUtf8();
    QString string = codec_1251->toUnicode(baPath);
    qDebug() << "BAPATH" << baPath;

    qDebug() << "STR" << sPath;
    if(QString::compare(sPath, rootPathLabel, Qt::CaseInsensitive) == 0) {
        qDebug() << "GO TO ROOT";
        filesCurrentView->setRootIndex(files->setRootPath(rootPath));
        return;
    }
    if (dPath.exists() && fPath.isDir()) {
        filesCurrentView->setRootIndex(files->setRootPath(sPath));
        ui->goToPath->setText(sPath);
    } else if (fPath.exists() && fPath.isFile()) {
        //Fix spaces and cyrillic symbols in path

                /*
                QUrl uPath;
                uPath.QUrl::setUrl(sPath);
                qDebug() << uPath;
                if (!QUrl(sPath).isValid()) {
                    qDebug() << "Invalid";
                    getValidPath(sPath);
                }
                */
                qDebug() << "Open File in LineEdit" <<sPath;
                QDesktopServices::openUrl(sPath);
    } else {
        QMessageBox::critical(this, "FileManager", "Не удалось перейти по указаному пути");
    }
}

void MainWindow::on_searchInCurDir_returnPressed() {

}

void MainWindow::removeKebab() {
    QString sPath = getPathByCurrentModelIndex();
    QFileInfo fPath = sPath;

    if(fPath.isDir()) {
        QDir dir(sPath);
        if(!dir.removeRecursively()) {
            QMessageBox::critical(this, "Delete", "Не удалось удалить указанную папку");
        }
    } else if(fPath.isFile()) {
        QFile file;
        if(!file.remove(sPath)) {
            QMessageBox::critical(this, "Delete", "Не удалось удалить указанный файл");
        }
    } else {
        QMessageBox::critical(this, "FileManager", "Не удалось перейти по указаному пути");
    }
}

void MainWindow::rename() {

    QString sPath = getPathByCurrentModelIndex();
    qDebug() << "ABS PATH" << sPath;
    QFileInfo fPath = sPath;
    QString newName, oldName = sPath;

    if (fPath.isDir()) {
        QDir directory;
        QString rawFileName = "name312312311224512";

        newName = sPath.section("/", 0, -2) + "/" + rawFileName;
        if(!directory.rename(oldName, newName)) {
            QMessageBox::critical(this, "Rename", "Rename error");
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
}


void MainWindow::on_CreateFile_triggered()
{
    DialogWindow dialogWindow(this, "Введите имя файла или папки", true);
    dialogWindow.setModal(true);
    dialogWindow.exec();
    //TODO
}

void MainWindow::dirUp() {
    QDir dPath = files->rootDirectory();
    QString sPath = files->rootPath();
    if(dPath.cdUp()) {
        int i = sPath.length();
        for (; sPath[i] != '/'; i--) {
            sPath[i] = sPath[i + 1];
        }
        qDebug() << "CDUP" << sPath;
        filesCurrentView->setRootIndex(files->setRootPath(sPath));
        ui->goToPath->setText(sPath);
    }
    else {
        dirRoot();
    }
    filesCurrentView->update();
}

void MainWindow::dirRoot() {
    ui->goToPath->setText(rootPathLabel);
    filesCurrentView->setRootIndex(files->setRootPath(rootPath));
}

void MainWindow::getValidPath(QString &path) {
#ifdef __linux__
    size_t found = path.find("%20");
    if (found != std::string::npos) {
        int i = found;
        while (i != path.length()) {
            size_t found = path.find("%20");
            if (found != std::string::npos) {
                i = found;
                path[i] = '\\';    i++;
                path[i] = ' ';          i++;
                for (int j = i; j < path.length(); j++) {
                    path[j] = path[j + 1];
                }
            }
            else {
                break;
            }
        }
    }
    qDebug() << path;
#elif _WIN32
    //Fix for WIN

#endif
}

void MainWindow::slotCustomMenuRequested(QPoint pos)
{
    /* Создаем объект контекстного меню */
    QMenu * contextMenu = new QMenu(this);

    QAction *openFileSystem = new QAction("Открыть");
    QAction *renameFileSystem = new QAction("Переименовать");
    QAction *copyFileSystem = new QAction("Копировать");
    QAction *cutFileSystem = new QAction("Вырезать");
    QAction *deleteFileSystem = new QAction("Удалить");
    QAction *pasteFileSystem = new QAction("Вставить");
    QAction *propertiesFileSystem = new QAction("Свойства");


    contextMenu->addAction(openFileSystem);
    contextMenu->addSeparator();

    contextMenu->addAction(copyFileSystem);
    contextMenu->addAction(cutFileSystem);
    contextMenu->addAction(pasteFileSystem);
    contextMenu->addSeparator();

    contextMenu->addAction(renameFileSystem);
    contextMenu->addAction(deleteFileSystem);
    contextMenu->addSeparator();

    contextMenu->addAction(propertiesFileSystem);

    connect(openFileSystem, &QAction::triggered, this, &MainWindow::fileSystemGoForward);
    connect(renameFileSystem, &QAction::triggered, this, &MainWindow::rename);
    connect(deleteFileSystem, &QAction::triggered, this, &MainWindow::removeKebab);


    /* Вызываем контекстное меню */
    contextMenu->popup(filesCurrentView->viewport()->mapToGlobal(pos));
}

QModelIndex MainWindow::getCurrentModelIndex() {
    if(filesCurrentView != nullptr) {
        return filesCurrentView->currentIndex();
    }
}

QString MainWindow::getPathByCurrentModelIndex() {
    QString path = files->filePath(getCurrentModelIndex());
    return path;
}
