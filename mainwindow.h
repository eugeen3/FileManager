#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifdef __linux__

#elif _WIN32
#include <Windows.h>
#endif

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
#include <QPoint>
#include <QProgressBar>
#include <QProcess>

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
    void cleanLayout(QLayout*);
    QWidget* getFilesWidget();
    void getValidPath(QString &);
    void resetModelIndex(QModelIndex &);
    QModelIndex getCurrentModelIndex();
    QString getPathByCurrentModelIndex();
    //void keyPressEvent(QKeyEvent *);
private:
    Ui::MainWindow *ui;
    QProcess *process;
    const QString rootPath;
    const QString rootPathLabel = "Компьютер";
    QFileSystemModel *directories;
    QFileSystemModel *files;
    QTableView *filesTable;
    QListView *filesList;
    QAbstractItemView *filesCurrentView;
    QCompleter *dirPath;

private slots:
    void on_treeView_clicked(const QModelIndex &);

   // void fileSystemGoForward(const QModelIndex &);
    void fileSystemGoForward();
    void removeKebab();
    void rename();

    void on_table_triggered();
    void on_list_triggered();

    void on_goToPath_returnPressed();
    void on_searchInCurDir_returnPressed();
    void on_CreateFile_triggered();
    void dirUp();
    void dirRoot();
    void slotCustomMenuRequested(QPoint);
signals:
    void enterPressed(const QModelIndex &);
    void deletePressed(const QModelIndex &);
};

#endif // MAINWINDOW_H
