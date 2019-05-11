#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "createfile.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString rootPath = QDir::rootPath();

    directories = new QFileSystemModel(this);
    directories->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    directories->setRootPath(QDir::rootPath());

    files = new QFileSystemModel(this);
    files->setFilter(QDir::Files | QDir::Dirs | QDir::NoDot);
    files->setRootPath(rootPath);

    ui->treeView->setModel(directories);
    listView = new QListView();
    listView->setModel(files);
    ui->rightlayout->addWidget(listView);

    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(3);
    ui->treeView->header()->hide();


    connect(listView, SIGNAL(doubleClicked()), this, SLOT(fileSystemDoubleClicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    QString path = directories->fileInfo(index).absoluteFilePath();
    listView->setRootIndex(files->setRootPath(path));
    ui->goToPath->setText(path);
}

void MainWindow::fileSystemDoubleClicked(const QModelIndex &index)
{
    QString path = files->fileInfo(index).absoluteFilePath();
    if(files->fileInfo(index).isDir()) {
        qDebug() << "STR" << path;
        listView->setRootIndex(files->setRootPath(path));
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

    if (listView == nullptr)
        listView = new QListView();

    if (files == nullptr) {
        files = new QFileSystemModel(this);
        files->setFilter(QDir::Files | QDir::Dirs | QDir::NoDot);
        files->setRootPath(QDir::rootPath());
    }

    listView->setModel(files);
    ui->rightlayout->addWidget(listView);
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
        listView->setRootIndex(files->setRootPath(sPath));
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
