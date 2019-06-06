#ifndef COPIER_H
#define COPIER_H

#include <QString>
#include <QThread>

class Copier : public QObject
{
    Q_OBJECT
protected:
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
};

#endif // COPIER_H
