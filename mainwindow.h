#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifdef __linux__

#elif _WIN32
#include <Windows.h>
#endif

#include <progressbarupdater.h>
#include <copier.h>
#include <dialogwindow.h>
#include <properties.h>
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
    //void cleanLayout(QLayout*);
    QWidget* getFilesWidget();
    void getValidPath(QString &);
    QModelIndex getCurrentModelIndex();
    QString getPathByCurrentModelIndex();
    void checkNewName();
    void setPrBarCurVal();
    QWidget* getParentWidget();
    //void setProgressBarMin(int);
   // void setProgressBarMax(int);
   // void setProgressBarCur(int);
   // QProgressBar& getProgressBar();
    //void keyPressEvent(QKeyEvent *);
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
    ProgressBarUpdater *prBarUpdater;
    QPushButton *cancel;
    bool isCutted;
    QString oldName;
    QString newName;
    QProgressBar *prBar;

private slots:
    void on_treeView_clicked(const QModelIndex &);
    void fileSystemGoForward();

    void copy();
    void cut();
    void paste();
    void rename();
    void removeOld();
    void remove();
    void showProperties();

    void on_goToPath_returnPressed();
    void on_CreateFile_triggered();
    void dirUp();
    void dirRoot();
    void slotCustomMenuRequested(QPoint);

signals:
    void enterPressed(const QModelIndex &);
    void deletePressed(const QModelIndex &);
};

#endif // MAINWINDOW_H
