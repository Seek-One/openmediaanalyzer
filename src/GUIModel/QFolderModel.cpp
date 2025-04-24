#include <QDebug>
#include <fstream>
#include <unistd.h>

#include "../GUI/QFolderView.h"

#include "QFolderModel.h"

QFolderModel::QFolderModel(){
    m_pModel = new QFileSystemModel();
}

QFolderModel::~QFolderModel(){
    if(m_pModel){
        delete m_pModel;
        m_pModel = nullptr;
    }
}

void QFolderModel::folderLoaded(const QString& folderPath){
    m_pModel->setRootPath(folderPath);
    m_loadedFolderPath = folderPath;
    emit updateFolderView(m_pModel);
    processDirectory(folderPath);
}

void QFolderModel::processFile(const QString& filePath){
    std::ifstream fileStream(filePath.toStdString(), std::ios::binary | std::ios::ate);
    if (!fileStream) {
        qDebug() << "Error opening file: " << filePath;
        return;
    }

    std::streamsize fileSize = fileStream.tellg();
    
    if(fileSize == 0) return;
    
    fileStream.seekg(0, std::ios::beg);
    uint8_t* buffer = new uint8_t[fileSize];
    fileStream.read(reinterpret_cast<char*>(buffer), fileSize);
    QString fileExtension = filePath.split('.').back(); 
    if(fileExtension == "h264") emit loadH264File(buffer, fileSize);
    else if (fileExtension == "h265") emit loadH265File(buffer, fileSize);
    else delete[] buffer;
    fileStream.close();
}

void QFolderModel::processDirectory(const QString& directoryPath){
    QDir dir(directoryPath);
    emit loadFolderStart();
    for(const QString& file : dir.entryList(QDir::Files | QDir::NoDotAndDotDot)){
        processFile(QString(directoryPath + "/" + file));
    }
    emit loadFolderEnd();
}