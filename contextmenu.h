#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QMenu>
#include "mainwindow.h"

class ContextMenu : public QMenu, public MainWindow
{
protected:
    QString copySource;
    QString copyDestination;
public:
    ContextMenu();
    bool copyFile(const QString &, const QString &);
    void copyDir(const QString &, const QString &);
private slots:
    void copy();
    //void cut();
    void paste();
    void rename();
    void removeKebab();
    void showProperties();
};

#endif // CONTEXTMENU_H
