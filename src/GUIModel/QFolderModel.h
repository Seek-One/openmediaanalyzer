#pragma once

#include <QObject>
#include <QFileSystemModel>

class QFolderView;

class QFolderModel : public QObject
{
    Q_OBJECT
public:
    QFolderModel();

    virtual ~QFolderModel();
    
signals:
    void updateFolderView(QFileSystemModel* model);
    void loadFile(uint8_t* fileContent, quint32 fileSize);
    void loadFolderStart();
    void loadFolderEnd();

public slots:
    void folderLoaded(const QString& folderPath);

private:
    QFileSystemModel* m_pModel;
    QString m_loadedFolderPath;

    void processFile(const QString& filePath);

    void processDirectory(const QString& directoryPath);
};