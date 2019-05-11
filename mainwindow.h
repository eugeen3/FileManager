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
    void cleanLayout(QLayout*);
public:
    Ui::MainWindow *ui;
    QFileSystemModel *directories;
    QFileSystemModel *files;
   // QTreeView *treeView;
    QTableView *tableView;
    QListView *listView;

private slots:
    void on_treeView_clicked(const QModelIndex &index);
    void fileSystemDoubleClicked(const QModelIndex &index);
    void on_table_triggered();
    void on_list_triggered();
    void on_goToPath_returnPressed();
    void on_searchInCurDir_returnPressed();
    void on_CreateFile_triggered();
signals:
    void doubleClicked(const QModelIndex &index);
};

#endif // MAINWINDOW_H
