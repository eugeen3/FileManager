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

    filesList = new QListView();
    filesList->setModel(files);
    ui->rightlayout->addWidget(filesList);

    ui->treeView->setModel(directories);
    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(3);
    ui->treeView->header()->hide();

    connect(filesList, &QAbstractItemView::doubleClicked, this, &MainWindow::fileSystemDoubleClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initFiles() {
    files = new QFileSystemModel(this);
    files->setFilter(QDir::Files | QDir::Dirs | QDir::NoDot);
    files->setRootPath(QDir::rootPath());
}

void MainWindow::initDirs() {
    directories = new QFileSystemModel(this);
    directories->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    directories->setRootPath(QDir::rootPath());
}

void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    QString path = directories->fileInfo(index).absoluteFilePath();
    filesList->setRootIndex(files->setRootPath(path));
    ui->goToPath->setText(path);
}

void MainWindow::fileSystemDoubleClicked(const QModelIndex &index)
{
    QString path = files->fileInfo(index).absoluteFilePath();
    if(files->fileInfo(index).isDir()) {
        qDebug() << "STR" << path;
        filesList->setRootIndex(files->setRootPath(path));
        ui->goToPath->setText(path);
    }
    else if (files->fileInfo(index).isFile()) {

        //Fix spaces and cyrillic symbols in path
        QDesktopServices::openUrl(path);
    }
}

void MainWindow::on_list_triggered()
{
    cleanLayout(ui->rightlayout);

    if (filesList == nullptr)
        filesList = new QListView();

    if (files == nullptr) {
        files = new QFileSystemModel(this);
        files->setFilter(QDir::Files | QDir::Dirs | QDir::NoDot);
        files->setRootPath(QDir::rootPath());
    }

    filesList->setModel(files);
    ui->rightlayout->addWidget(filesList);
}

void MainWindow::on_table_triggered()
{
    cleanLayout(ui->rightlayout);

    if (tableView == nullptr)
        tableView = new QTableView();

    if (files == nullptr) {
        files = new QFileSystemModel(this);
        files->setFilter(QDir::Files | QDir::Dirs | QDir::NoDot);
        files->setRootPath(QDir::rootPath());
    }

    tableView->setModel(files);
    tableView->verticalHeader()->hide();
    ui->rightlayout->addWidget(tableView);
}

void MainWindow::cleanLayout(QLayout *layout) {
    QLayoutItem *item;
    item = layout->itemAt(1);
    layout->removeItem(item);
    layout->removeWidget(item->widget());
    delete item;
}

void MainWindow::on_goToPath_returnPressed()
{
    QString sPath = ui->goToPath->text();
    QDir dPath = ui->goToPath->text();
    QFileInfo fPath = ui->goToPath->text();

    qDebug() << "STR" << sPath;
    if (dPath.exists() && fPath.isDir()){
        filesList->setRootIndex(files->setRootPath(sPath));
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
