#ifndef COPIER_H
#define COPIER_H

//#include <QMessageBox>
#include <QString>
#include <QThread>

class Copier : public QObject
{
    Q_OBJECT
protected:
    bool isFinished;
    QString copySource;
    QString copyDestination;
private:
    QThread *thread;
public:
    Copier();
    ~Copier();
    void copyFile(const QString &, const QString &);
    void copyDir(const QString &, const QString &);
    void paste();
    void startCopy();
    void setDestinationPath(const QString &);
    QString getDestinationPath();
    void setSourcePath(const QString &);
    QString getSourcePath();
signals:
    void copyFinished();
private slots:
    void changeState();
};

#endif // COPIER_H
