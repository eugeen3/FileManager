#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString rootPath = QDir::rootPath();
    //QString rootPathLabel = "Компьютер";

    initFiles();
    initDirs();
    //listView();
    tableView();
    initTree();

    dirPath = new QCompleter(files);
    ui->goToPath->setCompleter(dirPath);
    ui->goToPath->setText(rootPathLabel);
    ui->rightLayout->addWidget(filesTable, 2, 0, 1, 3);
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

    creating = false;
    connect(ui->createFile, &QAction::triggered, this, createNewTriggered);
    connect(ui->createDir, &QAction::triggered, this, createNewTriggered);
    connect(ui->nameEdit, &QLineEdit::returnPressed, this, lineEditUsingFor);
    connect(filesTable, &QWidget::customContextMenuRequested, this, &MainWindow::slotCustomMenuRequested);
    connect(filesTable, &QAbstractItemView::doubleClicked, this, &MainWindow::fileSystemGoForward);
    connect(ui->goBack, &QPushButton::clicked, this, &MainWindow::dirUp);
    connect(ui->toTheRoot, &QPushButton::clicked, this, &MainWindow::dirRoot);
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


void MainWindow::keyPressEvent(QKeyEvent *keyEvent) {
    switch (keyEvent->key()) {
        case Qt::Key_Enter : fileSystemGoForward();
        //case Qt::Key_Delete : emit deletePressed();
    }
}


void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    QString path = directories->fileInfo(index).absoluteFilePath();
    filesTable->setRootIndex(files->setRootPath(path));
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
        filesTable->setRootIndex(files->setRootPath(sPath));
        ui->goToPath->setText(sPath);
    }
    else if (files->fileInfo(index).isFile()) {
        openFile(sPath);
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
        filesTable->setRootIndex(files->setRootPath(rootPath));
        return;
    }
    if (dPath.exists() && fPath.isDir()) {
        filesTable->setRootIndex(files->setRootPath(sPath));
        ui->goToPath->setText(sPath);
    } else if (fPath.exists() && fPath.isFile()) {
        openFile(sPath);
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
    ui->nameEdit->setText(oldName);
}

void MainWindow::createNew() {
    QString name = ui->nameEdit->text();
    if (!name.contains(".")) {
        QDir newDir(ui->goToPath->text() + QDir::separator() + name);
        if (newDir.exists()) {
            ui->nameEdit->clear();
            QMessageBox::critical(this, "Ошибка переименования", "Папка с указанным именем уже существует");
        }
        else newDir.mkdir(ui->goToPath->text() + QDir::separator() + name);
    }
    else {
        QFile newFile(ui->goToPath->text() + QDir::separator() + name);
        if (newFile.exists()) {
            ui->nameEdit->clear();
            QMessageBox::critical(this, "Ошибка переименования", "Папка с указанным именем уже существует");
        }
        else {
            newFile.open(QIODevice::WriteOnly);
        }
    }
    ui->nameEdit->clear();
    creating = false;
}

void MainWindow::checkNewName() {
    QFileInfo fPath = oldName;
    qDebug() << "ABS PATH" << oldName;
    QString newName;
    if (fPath.isDir()) {
        QDir directory;
        if(!directory.rename(oldName, ui->nameEdit->text())) {
            ui->nameEdit->setText(oldName);
            QMessageBox::critical(this, "Ошибка переименования", "Папка с указанным именем уже существует");
        }
    }
    else if (fPath.isFile()) {
        QFile file;
        if(!file.rename(oldName, ui->nameEdit->text())) {
            ui->nameEdit->setText(oldName);
            QMessageBox::critical(this, "Ошибка переименования", "Файл с указанным именем уже существует");
        }
    }
}

void MainWindow::showProperties() {
    QThread *props = new QThread();
    Properties *propWin = new Properties(new QFileInfo(getPathByCurrentModelIndex()));
    propWin->moveToThread(props);
    propWin->exec();
}

void MainWindow::dirUp() {
    QDir dPath = files->rootDirectory();
    QString sPath = files->rootPath();
    if(dPath.cdUp()) {
        sPath = sPath.section("/", 0, -2);
        qDebug() << "CDUP" << sPath;
        filesTable->setRootIndex(files->setRootPath(sPath));
        ui->goToPath->setText(sPath);
    }
    else {
        dirRoot();
    }
    filesTable->update();
}

void MainWindow::dirRoot() {
    ui->goToPath->setText(rootPathLabel);
    filesTable->setRootIndex(files->setRootPath(rootPath));
}

void MainWindow::openFile(QString path) {
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
    QString program;
    QString initCmd;

    if (path.contains(" ")) {
        std::string temp = path.toUtf8().constData();
        // std::string current_locale_text = qs.toLocal8Bit().constData();
        //QString temp = sPath;
        //std::string temp = '"' + const_cast<string>(sPath);
        temp += '"';
        std::string temp2 = '"' + temp;
        path = QString::fromStdString(temp2);
        qDebug() << "SPath " << path;
    }
    program = "C:\\Windows\\System32\\cmd.exe";
    initCmd = "cmd /C";
    path.replace("/", "\\");
    initCmd += path;
    process = new QProcess();
    QStringList args;
#endif
    args.append(initCmd);
    qDebug() << "Open File" <<path;
    process->setProgram(program);
    process->setArguments(args);
    process->startDetached();
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

    contextMenu->popup(filesTable->viewport()->mapToGlobal(pos));
}

QModelIndex MainWindow::getCurrentModelIndex() {
    return filesTable->currentIndex();
}

QString MainWindow::getPathByCurrentModelIndex() {
    return files->filePath(getCurrentModelIndex());
}

void MainWindow::lineEditUsingFor() {
    if (creating) createNew();
    else checkNewName();
}

void MainWindow::createNewTriggered()
{
    ui->nameEdit->clear();
    QFocusEvent* eventFocus = new QFocusEvent(QEvent::FocusIn);
    qApp->postEvent(ui->nameEdit, (QEvent *)eventFocus, Qt::LowEventPriority);
    ui->nameEdit->grabKeyboard();
    creating = true;
}
