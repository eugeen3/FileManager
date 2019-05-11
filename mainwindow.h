#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QTreeView>
#include <QFileSystemModel>
#include <QDesktopServices>
#include <QUrl>
#include <QTableView>
#include <QTreeView>
#include <QListView>
#include <QMessageBox>
#include <QtDebug>
#include <qlistview.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initList();
    void initTable();
    void initFiles();
    void initDirs();
    void cleanLayout(QLayout*);

private:
    Ui::MainWindow *ui;
    QFileSystemModel *directories;
    QFileSystemModel *files;
    QTableView *tableView;
    QListView *filesList;

private slots:
    void on_treeView_clicked(const QModelIndex &);
    void fileSystemDoubleClicked(const QModelIndex &);
    void on_table_triggered();
    void on_list_triggered();
    void on_goToPath_returnPressed();
    void on_searchInCurDir_returnPressed();
    void on_CreateFile_triggered();
};

#endif // MAINWINDOW_H
