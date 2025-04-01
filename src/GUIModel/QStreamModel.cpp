#include <QDebug>
#include <QCoreApplication>
#include <QThread>
#include <fstream>

#include "../GUI/QFolderView.h"

#include "QStreamModel.h"

QStreamModel::QStreamModel():
    m_processing(false), m_loadedFolderPath("")
{
    m_pModel = new QFileSystemModel();
}

QStreamModel::~QStreamModel(){
    if(m_pModel){
        delete m_pModel;
        m_pModel = nullptr;
    }
}

void QStreamModel::streamLoaded(const QString& folderPath){
    qDebug() << folderPath;
    m_loadedFolderPath = folderPath;
    m_loadedDir = QDir(folderPath);
    m_pModel->setRootPath(m_loadedFolderPath);
    emit updateFolderView(m_pModel);
    emit loadFolderStart();
    m_processing.storeRelaxed(true);
}

void QStreamModel::run(){
    m_running.storeRelaxed(true);
    m_processing.storeRelaxed(false);
    while(m_running.loadRelaxed()){
        QCoreApplication::processEvents();
        if(m_processing.loadRelaxed() && !m_loadedFolderPath.isEmpty()){
            while(m_processing.loadRelaxed() && m_running.loadRelaxed()){
                for(const QString& file : m_loadedDir.entryList(QDir::Files | QDir::NoDotAndDotDot)){
                    QCoreApplication::processEvents();
                    processFile(QString(m_loadedFolderPath + "/" + file));
                    if(!m_processing.loadRelaxed() || !m_running.loadRelaxed()) break;
                    QThread::msleep(100);
                }
            }
        }
    }   
}

void QStreamModel::stopProcessing(){
    m_processing.storeRelaxed(false);
}

void QStreamModel::stopRunning(){
    m_running.storeRelaxed(false);
}

void QStreamModel::processFile(const QString& filePath){
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
    emit loadFile(buffer, fileSize);
    fileStream.close();
}

void QStreamModel::processDirectory(const QString& directoryPath){
    m_pModel->setRootPath(directoryPath);
    emit updateFolderView(m_pModel);
    QDir dir(directoryPath);
    emit loadFolderStart();
    for(const QString& file : dir.entryList(QDir::Files | QDir::NoDotAndDotDot)){
        processFile(QString(directoryPath + "/" + file));
    }
}