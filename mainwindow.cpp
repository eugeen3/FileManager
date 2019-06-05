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
    listView();
    // tableView();
    initTree();

    dirPath = new QCompleter(files);
    ui->goToPath->setCompleter(dirPath);
    ui->goToPath->setText(rootPathLabel);
    filesCurrentView = filesList;
    ui->rightLayout->addWidget(filesCurrentView, 2, 0, 1, 3);
    ui->splitter->setStretchFactor(1, 1);

    prBar = new QProgressBar(this);
    ui->statusBar->addWidget(prBar);
    cancel = new QPushButton(this);
    cancel->setMinimumWidth(200);
    cancel->setEnabled(false);
    cancel->setText("");
    ui->statusBar->addWidget(cancel);

    copier = new Copier();
    prBarUpdater = new ProgressBarUpdater();

    connect(filesCurrentView, &QWidget::customContextMenuRequested, this, &MainWindow::slotCustomMenuRequested);
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

void MainWindow::listView() {
    filesList = new QListView();
    filesList->setModel(files);
    filesList->setSelectionMode(QAbstractItemView::SingleSelection);
    filesList->setViewMode(QListView::ListMode);
    filesList->setContextMenuPolicy(Qt::CustomContextMenu);
}

void MainWindow::tableView() {
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
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
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

        if (sPath.contains(" ")) {
            QString temp = sPath;
            //std::string temp = '"' + sPath;
            sPath = temp += '"';
            qDebug() << "SPath " << sPath;
        }
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
        // cleanLayout(ui->rightLayout);

        if (filesList == nullptr) listView();
        ui->rightLayout->replaceWidget(filesCurrentView, filesList);

        filesCurrentView = filesList;
        //    ui->rightLayout->addWidget(filesList, 2, 0, 1, 3);
        connect(filesCurrentView, &QAbstractItemView::doubleClicked, this, &MainWindow::fileSystemGoForward);
    }
    filesList->setViewMode(QListView::ListMode);
}

void MainWindow::on_table_triggered()
{
    if (filesCurrentView != filesTable) {
        disconnect(filesCurrentView, &QAbstractItemView::doubleClicked, this, &MainWindow::fileSystemGoForward);
        //  cleanLayout(ui->rightLayout);

        if (filesTable == nullptr) tableView();
        ui->rightLayout->replaceWidget(filesCurrentView, filesTable);

        filesCurrentView = filesTable;
        //  ui->rightLayout->addWidget(filesTable, 2, 0, 1, 3);
        connect(filesCurrentView, &QAbstractItemView::doubleClicked, this, &MainWindow::fileSystemGoForward);
    }
}

void MainWindow::on_icons_triggered()
{
    filesList->setViewMode(QListView::IconMode);
    filesList->setIconSize(QSize(128, 128));
    //  filesCurrentView = filesIcon;
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

void MainWindow::remove() {
    QString sPath = getPathByCurrentModelIndex();
    QFileInfo fPath = sPath;

    if(fPath.isDir()) {
        QDir dir(sPath);
        if(!dir.removeRecursively()) {
            QMessageBox::critical(this, "Удаление", "Не удалось удалить указанную папку");
        }
    } else if(fPath.isFile()) {
        QFile file;
        if(!file.remove(sPath)) {
            QMessageBox::critical(this, "Удаление", "Не удалось удалить указанный файл");
        }
    }
}

void MainWindow::copy() {
    copier->setSourcePath(getPathByCurrentModelIndex());
    qDebug() << "Copy from" << getPathByCurrentModelIndex();
}

void MainWindow::cut() {
    isCutted = true;
    copy();
}

void MainWindow::removeOld() {
    if (isCutted == true) {
        QString sPath = copier->getSourcePath();
        QFileInfo fPath = sPath;

        if(fPath.isDir()) {
            QDir dir(sPath);
            if(!dir.removeRecursively()) {
                QMessageBox::critical(this, "Удаление", "Не удалось удалить указанную папку");
            }
        } else if(fPath.isFile()) {
            QFile file;
            if(!file.remove(sPath)) {
                QMessageBox::critical(this, "Удаление", "Не удалось удалить указанный файл");
            }
        }
    }
}

void MainWindow::paste() {
    prBar->setMinimum(0);
    copier->setDestinationPath(getPathByCurrentModelIndex());
    prBar->setMaximum(static_cast<int>(prBarUpdater->getSourceSize()));
    copier->startCopy();
}

void MainWindow::setPrBarCurVal() {
    while(prBarUpdater->checkState()) {
        prBar->setValue(static_cast<int>(prBarUpdater->getDestinationSize()));
    }
}

void MainWindow::rename() {
    oldName = getPathByCurrentModelIndex();
    ui->searchInCurDir->setText(oldName);
    QFileInfo fPath = oldName;
    qDebug() << "ABS PATH" << oldName;
    QString newName;
    if (fPath.isDir()) {
        QDir directory;
        QString rawFileName;
        newName = oldName.section("/", 0, -2) + "/" + rawFileName;
    }
    else if (fPath.isFile()) {
        QFile file;
        QString rawFileName = "name312312312.txt";

        newName = oldName.section("/", 0, -2) + "/" + rawFileName;
        if(!file.rename(oldName, newName)) {
            QMessageBox::critical(this, "Rename", "Rename error");
        }
    } else {
        QMessageBox::critical(this, "FileManager", "Не удалось перейти по указаному пути");
    }
}

void MainWindow::checkNewName() {
   /* while(!rename(oldName, ui->searchInCurDir->text())) {
          QMessageBox::critical(this, "Rename", "Rename error");
    }*/
}

void MainWindow::showProperties() {
    QThread *props = new QThread();
    Properties *propWin = new Properties(new QFileInfo(getPathByCurrentModelIndex()));
    propWin->moveToThread(props);
    propWin->exec();
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
    QMenu * contextMenu = new QMenu(this);
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
    QAction *pasteFileSystem = new QAction("Вставить");

    if (copier->getSourcePath() == nullptr) {
        pasteFileSystem->setEnabled(false);
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
    connect(copyFileSystem, &QAction::triggered, this, &MainWindow::copy);
    connect(cutFileSystem, &QAction::triggered, this, &MainWindow::cut);
    connect(renameFileSystem, &QAction::triggered, this, &MainWindow::rename);
    connect(deleteFileSystem, &QAction::triggered, this, &MainWindow::remove);
    connect(propertiesFileSystem, &QAction::triggered, this, &MainWindow::showProperties);
    connect(copier, &Copier::copyFinished, this, &MainWindow::removeOld);

    contextMenu->popup(filesCurrentView->viewport()->mapToGlobal(pos));
}

QModelIndex MainWindow::getCurrentModelIndex() {
    return filesCurrentView->currentIndex();
}

QString MainWindow::getPathByCurrentModelIndex() {
    return files->filePath(getCurrentModelIndex());
}
/*
bool MainWindow::checkNewName() {
    if ()
}
*/

QWidget* MainWindow::getParentWidget() {
    return this;
}
/*
void MainWindow::setProgressBarMin(int min) {
    prBar->setMinimum(min);
}

void MainWindow::setProgressBarMax(int max) {
    prBar->setMaximum(max);
}

void MainWindow::setProgressBarCur(int cur) {
    prBar->setValue(cur);
}
*/
