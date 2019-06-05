#include "progressbarupdater.h"

ProgressBarUpdater::ProgressBarUpdater()
{
    updater = new QThread();
    this->moveToThread(updater);
    updater->start();
}

qint64 ProgressBarUpdater::dirSize(QString dirPath) {
    qint64 size = 0;
    QDir dir(dirPath);

    QDir::Filters fileFilters = QDir::Files|QDir::System|QDir::Hidden;
    for(QString filePath : dir.entryList(fileFilters)) {
        QFileInfo fi(dir, filePath);
        size+= fi.size();
    }

    QDir::Filters dirFilters = QDir::Dirs|QDir::NoDotAndDotDot|QDir::System|QDir::Hidden;
    for(QString childDirPath : dir.entryList(dirFilters)) {
        size+= dirSize(dirPath + QDir::separator() + childDirPath);
    }

    return size;
}

qint64 ProgressBarUpdater::getSourceSize() {
    return dirSize(copySource);
}

qint64 ProgressBarUpdater::getDestinationSize() {
    return dirSize(copyDestination);
}

bool ProgressBarUpdater::checkState() {
    return isFinished;
}
