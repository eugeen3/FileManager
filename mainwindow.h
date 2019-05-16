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
#include <cstring>
#include <algorithm>
#include <QPushButton>
#include <QTextCodec>
#include <QCompleter>
#include <QKeyEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initFilesList();
    void initFilesTable();
    void initFiles();
    void initDirs();
    void initTree();
    void initButtons();
    void cleanLayout(QLayout*);
    QWidget* getFilesWidget();
    void getValidPath(QString &);
    void keyPressEvent(QKeyEvent *);
private:
    Ui::MainWindow *ui;
    const QString rootPath;
    const QString rootPathLabel = "Компьютер";
    QFileSystemModel *directories;
    QFileSystemModel *files;
    QTableView *filesTable;
    QListView *filesList;
    QAbstractItemView *filesCurrentView;
    QPushButton *goBack;
    QPushButton *toTheRoot;
    QCompleter *dirPath;
    int viewModel;

private slots:
    void on_treeView_clicked(const QModelIndex &);
    void fileSystemGoForward(const QModelIndex &);
    void removeKebab(const QModelIndex &);
    void on_table_triggered();
    void on_list_triggered();
    void on_goToPath_returnPressed();
    void on_searchInCurDir_returnPressed();
    void on_CreateFile_triggered();
    void dirUp();
    void dirRoot();
signals:
    void enterPressed(const QModelIndex &);
    void deletePressed(const QModelIndex &);
};

#endif // MAINWINDOW_H
