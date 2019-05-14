#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "createfile.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString rootPath = QDir::rootPath();

    initFiles();
    initDirs();
    initButtons();
    //initFilesList();
    //ui->rightlayout->addWidget(filesList);
    //viewModel = 1;
    initFilesTable();
    ui->rightLayout->addWidget(filesTable, 2, 0, 1, 2);
    viewModel = 2;

    initTree();

    //connect(filesList, &QAbstractItemView::doubleClicked, this, &MainWindow::fileSystemDoubleClicked);
    connect(filesTable, &QAbstractItemView::doubleClicked, this, &MainWindow::fileSystemDoubleClicked);
    connect(goBack, &QPushButton::clicked, this, &MainWindow::dirUp);
    connect(toTheRoot, &QPushButton::clicked, this, &MainWindow::dirRoot);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initFiles() {
    files = new QFileSystemModel(this);
    files->setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    files->setRootPath(QDir::rootPath());
}

void MainWindow::initDirs() {
    directories = new QFileSystemModel(this);
    directories->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    directories->setRootPath(QDir::rootPath());
}

void MainWindow::initFilesList() {
    filesList = new QListView();
    filesList->setModel(files);
}

void MainWindow::initFilesTable() {
    filesTable = new QTableView();
    filesTable->setModel(files);
    filesTable->verticalHeader()->hide();
    filesTable->setSortingEnabled(true);
    filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    filesTable->setSelectionMode(QAbstractItemView::SingleSelection);
}

void MainWindow::initTree() {
    ui->treeView->setModel(directories);
    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(3);
    ui->treeView->header()->hide();
}

void MainWindow::initButtons() {
    goBack = new QPushButton("Назад");
    goBack->setMinimumSize(170, 20);

    toTheRoot = new QPushButton("Корневая папка");
    toTheRoot->setMinimumSize(170, 20);

    ui->rightLayout->addWidget(goBack, 1, 0);
    ui->rightLayout->addWidget(toTheRoot, 1, 1);
}

void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    QString path = directories->fileInfo(index).absoluteFilePath();
    switch (viewModel)
    {
    case 1: filesList->setRootIndex(files->setRootPath(path));
    case 2: filesTable->setRootIndex(files->setRootPath(path));
    }
    ui->goToPath->setText(path);
}

void MainWindow::fileSystemDoubleClicked(const QModelIndex &index)
{
    QString path = files->fileInfo(index).absoluteFilePath();
    if(files->fileInfo(index).isDir()) {
        qDebug() << "STR" << path;
        switch (viewModel)
        {
        case 1: filesList->setRootIndex(files->setRootPath(path));
        case 2: filesTable->setRootIndex(files->setRootPath(path));
        }
        ui->goToPath->setText(path);
    }
    else if (files->fileInfo(index).isFile()) {
        getValidPath(path);
        //Fix spaces and cyrillic symbols in path
        QDesktopServices::openUrl(path);
    }
}

void MainWindow::on_list_triggered()
{
    cleanLayout(ui->rightLayout);
    if(filesTable != nullptr)
            disconnect(filesTable, &QAbstractItemView::doubleClicked,
                       this, &MainWindow::fileSystemDoubleClicked);

    if (files == nullptr) initFiles();
    if (filesList == nullptr) initFilesList();

    connect(filesList, &QAbstractItemView::doubleClicked, this, &MainWindow::fileSystemDoubleClicked);
    ui->rightLayout->addWidget(filesList);
}

void MainWindow::on_table_triggered()
{
    cleanLayout(ui->rightLayout);
    if(filesTable != nullptr)
            disconnect(filesList, &QAbstractItemView::doubleClicked,
                       this, &MainWindow::fileSystemDoubleClicked);

    if (files == nullptr) initFiles();
    if (filesList == nullptr) initFilesTable();

    connect(filesTable, &QAbstractItemView::doubleClicked, this, &MainWindow::fileSystemDoubleClicked);
    ui->rightLayout->addWidget(filesTable);
}

void MainWindow::cleanLayout(QLayout *layout) {
    if (layout) {
        if(layout->count() > 1){
                QLayoutItem *item = layout->takeAt(1);
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

    qDebug() << "STR" << sPath;
    if (dPath.exists() && fPath.isDir()){
        switch (viewModel)
        {
        case 1: filesList->setRootIndex(files->setRootPath(sPath));
        case 2: filesTable->setRootIndex(files->setRootPath(sPath));
        }
        ui->goToPath->setText(sPath);
    } else if (fPath.exists() && fPath.isFile()) {
        //Fix spaces and cyrillic symbols in path
        QDesktopServices::openUrl(sPath);
    } else {
        QMessageBox::critical(this, "FileManager", "Не удалось перейти по указаному пути");
    }
}

void MainWindow::on_searchInCurDir_returnPressed()
{

}

void MainWindow::on_CreateFile_triggered()
{
    CreateFile dialogWindow;
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
        sPath[i] = sPath[i + 1];
        qDebug() << "CDUP" << sPath;
        switch (viewModel)
        {
        case 1: filesList->setRootIndex(files->setRootPath(sPath));
        case 2: filesTable->setRootIndex(files->setRootPath(sPath));
        }
    }
}

void MainWindow::dirRoot() {
    QString sPath = QDir::rootPath();
    switch (viewModel)
    {
    case 1: filesList->setRootIndex(files->setRootPath(sPath));
    case 2: filesTable->setRootIndex(files->setRootPath(sPath));
    }
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


