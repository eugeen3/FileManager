#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifdef __linux__

#elif _WIN32
#include <Windows.h>
#endif

#include <copier.h>
#include <properties.h>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QTreeView>
#include <QFileSystemModel>
#include <QDesktopServices>
#include <QUrl>
#include <QTableView>
#include <QTreeView>
#include <QMessageBox>
#include <qlistview.h>
#include <cstring>
#include <algorithm>
#include <QPushButton>
#include <QTextCodec>
#include <QCompleter>
#include <QKeyEvent>
#include <QPoint>
#include <QProcess>
#include <QThread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void tableView();
    void initFiles();
    void initDirs();
    void initTree();
    void openFile(QString);
    QModelIndex getCurrentModelIndex();
    QString getPathByCurrentModelIndex();
    void createNew();

private:
    Ui::MainWindow *ui;
    QProcess *process;
    const QString rootPath;
    const QString rootPathLabel = "";
    QFileSystemModel *directories;
    QFileSystemModel *files;
    QTableView *filesTable;
    QCompleter *dirPath;
    Copier *copier;
    bool isCutted;
    bool creating;
    QString oldName;
    QString newName;

private slots:
    void on_treeView_clicked(const QModelIndex &);
    void fileSystemGoForward();

    void copy();
    void cut();
    void paste();
    void rename();
    void checkNewName();
    void removeOld();
    void remove();
    void showProperties();

    void on_goToPath_returnPressed();
    void dirUp();
    void dirRoot();
    void slotCustomMenuRequested(QPoint);
    void lineEditUsingFor();
    void createNewTriggered();
};

#endif // MAINWINDOW_H
