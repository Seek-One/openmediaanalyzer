#pragma once

#include <QObject>
#include <QFileSystemModel>

#include "QAccessUnitModel.h"
#include "../../lib/Threading/QWorkerObject.h"

class QFolderView;

class QStreamModel : public QWorkerObject
{
    Q_OBJECT
public:
    QStreamModel();

    virtual ~QStreamModel();
    
signals:
    void updateFolderView(QFileSystemModel* model);
    void loadFile(uint8_t* fileContent, quint32 fileSize);
    void loadFolderStart();

protected:
    void run();

public slots:
    void streamLoaded(const QString& folderPath);
    void stopProcessing();
    void stopRunning();

private:
    QFileSystemModel* m_pModel;
    QString m_loadedFolderPath;
    QDir m_loadedDir;
    QAtomicInteger<bool> m_processing;
    QAtomicInteger<bool> m_running;

    void processFile(const QString& filePath);

    void processDirectory(const QString& directoryPath);
};