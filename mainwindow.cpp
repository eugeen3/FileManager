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

    //treeView = new QTreeView(this);
    //filesList = new QListView(this);
    ui->treeView->setModel(directories);
    ui->listView->setModel(files);
    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(3);
    ui->treeView->header()->hide();

    //ui->horizontalLayout_2->addWidget(filesList);
    //ui->horizontalLayout->addWidget(treeView);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    QString path = directories->fileInfo(index).absoluteFilePath();
    ui->listView->setRootIndex(files->setRootPath(path));
    ui->goToPath->setText(path);
}

void MainWindow::on_listView_doubleClicked(const QModelIndex &index)
{
    QString path = files->fileInfo(index).absoluteFilePath();
    if(files->fileInfo(index).isDir()) {
        qDebug() << "STR" << path;
        ui->listView->setRootIndex(files->setRootPath(path));
        ui->goToPath->setText(path);
    }
    else if (files->fileInfo(index).isFile()) {

        //Fix spaces and cyrillic symbols in path
        QDesktopServices::openUrl(path);
    }
}
/*
void MainWindow::on_list_triggered()
{
    cleanLayout(ui->horizontalLayout_2);
    //ui->horizontalLayout_2->removeWidget(this);
    //delete this;
    if (filesList == nullptr)
        filesList = new QListView();
    ui->horizontalLayout_2->addWidget(filesList);
}

void MainWindow::on_tree_triggered()
{
    cleanLayout(ui->horizontalLayout_2);
    //ui->horizontalLayout_2->removeWidget(this);
    //delete this;
    if (filesTable == nullptr)
        filesTable = new QTableView();
    ui->horizontalLayout_2->addWidget(filesTable);
}

void MainWindow::cleanLayout(QLayout *layout) {
    QLayoutItem *item;
    while( (item = layout->itemAt(0)) )
        {
                layout->removeItem( item );
                layout->removeWidget(item->widget());
                //delete item->widget();
                delete item;
                //layout->update();
        }
}
*/

void MainWindow::on_goToPath_returnPressed()
{
    QString sPath = ui->goToPath->text();
    QDir dPath = ui->goToPath->text();
    QFileInfo fPath = ui->goToPath->text();

    qDebug() << "STR" << sPath;
    if (dPath.exists() && fPath.isDir()){
        ui->listView->setRootIndex(files->setRootPath(sPath));
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
