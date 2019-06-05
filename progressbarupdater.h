#ifndef PROGRESSBARUPDATER_H
#define PROGRESSBARUPDATER_H

#include <copier.h>
#include <QFileInfo>
#include <QDir>

class ProgressBarUpdater : public Copier
{
private:
    QThread *updater;
public:
    ProgressBarUpdater();
    qint64 dirSize(QString dirPath);
    qint64 getSourceSize();
    qint64 getDestinationSize();
    bool checkState();
};

#endif // PROGRESSBARUPDATER_H
