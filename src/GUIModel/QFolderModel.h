#pragma once

#include <QObject>
#include <QFileSystemModel>

class QVideoInputView;

class QFolderModel : public QObject
{
    Q_OBJECT
public:
    QFolderModel();

    virtual ~QFolderModel();
    
signals:
    void updateVideoInputView(QFileSystemModel* model);
    void loadH264File(uint8_t* fileContent, quint32 fileSize);
    void loadH265File(uint8_t* fileContent, quint32 fileSize);
    void loadFolderStart(bool isLiveStream);
    void loadFolderEnd();

public slots:
    void folderLoaded(const QString& folderPath);

private:
    QFileSystemModel* m_pModel;
    QString m_loadedFolderPath;

    void processFile(const QString& filePath);

    void processDirectory(const QString& directoryPath);
};